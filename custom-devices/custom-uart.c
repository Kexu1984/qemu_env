/*
 * Custom UART emulation for learning purposes
 *
 * This is a simple UART implementation that demonstrates how to create
 * custom peripherals in QEMU.
 *
 * Memory Map:
 * 0x00: DATA    - Data register (read/write)
 * 0x04: STATUS  - Status register (read-only)
 * 0x08: CONTROL - Control register (read/write)
 */

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "qapi/error.h"
#include "hw/sysbus.h"
#include "migration/vmstate.h"
#include "hw/registerfields.h"
#include "chardev/char-fe.h"
#include "hw/irq.h"
#include "qemu/timer.h"
#include "hw/qdev-properties.h"

#define TYPE_CUSTOM_UART "custom-uart"

typedef struct CustomUartState {
    SysBusDevice parent_obj;
    MemoryRegion mmio;
    CharBackend chr;
    qemu_irq irq;
    uint8_t  data;
    uint32_t status;
    uint32_t control;
    uint8_t rx_fifo[16];
    int rx_count;
} CustomUartState;

#define CUSTOM_UART(obj) OBJECT_CHECK(CustomUartState, (obj), TYPE_CUSTOM_UART)

/* Register definitions */
#define UART_DATA    0x00
#define UART_STATUS  0x04
#define UART_CONTROL 0x08

/* Status register bits */
#define STATUS_TX_READY  (1 << 0)  /* Transmit ready */
#define STATUS_RX_AVAIL  (1 << 1)  /* Receive data available */

/* Control register bits */
#define CTRL_TX_EN       (1 << 0)  /* Transmit enable */
#define CTRL_RX_EN       (1 << 1)  /* Receive enable */
#define CTRL_TX_INT_EN   (1 << 2)  /* Transmit interrupt enable */
#define CTRL_RX_INT_EN   (1 << 3)  /* Receive interrupt enable */

static uint64_t custom_uart_read(void *opaque, hwaddr offset, unsigned size)
{
    CustomUartState *s = CUSTOM_UART(opaque);
    uint64_t retval = 0;

    switch (offset) {
    case UART_DATA:
        if (s->rx_count > 0) {
            retval = s->rx_fifo[0];
            /* Shift FIFO */
            for (int i = 0; i < s->rx_count - 1; i++) {
                s->rx_fifo[i] = s->rx_fifo[i + 1];
            }
            s->rx_count--;
            if (s->rx_count == 0) {
                s->status &= ~STATUS_RX_AVAIL;
            }
        }
        printf("Custom UART: Read DATA register, value=0x%x\n", (uint32_t)retval);
        break;
        
    case UART_STATUS:
        retval = s->status;
        printf("Custom UART: Read STATUS register, value=0x%x\n", (uint32_t)retval);
        break;
        
    case UART_CONTROL:
        retval = s->control;
        printf("Custom UART: Read CONTROL register, value=0x%x\n", (uint32_t)retval);
        break;
        
    default:
        qemu_log_mask(LOG_GUEST_ERROR, 
                      "custom-uart: invalid read offset 0x%x\n", (int)offset);
        break;
    }
    
    return retval;
}

static void custom_uart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)
{
    CustomUartState *s = CUSTOM_UART(opaque);

    switch (offset) {
    case UART_DATA:
        printf("Custom UART: Write to DATA register, value=0x%x, control=0x%x\n", 
               (uint32_t)value, s->control);
        if (s->control & CTRL_TX_EN) {
            /* Send character to backend */
            uint8_t ch = value & 0xFF;
            printf("Custom UART: Sending char 0x%02x ('%c')\n", ch, 
                   (ch >= 32 && ch < 127) ? ch : '?');
            
            int ret = qemu_chr_fe_write_all(&s->chr, &ch, 1);
            printf("Custom UART: qemu_chr_fe_write_all returned %d\n", ret);
            
            /* Set transmit ready status */
            s->status |= STATUS_TX_READY;
            
            /* Generate interrupt if enabled */
            if (s->control & CTRL_TX_INT_EN) {
                qemu_irq_raise(s->irq);
            }
        } else {
            printf("Custom UART: TX not enabled, ignoring write\n");
        }
        break;
        
    case UART_CONTROL:
        s->control = value;
        printf("Custom UART: Control register set to 0x%x\n", (uint32_t)value);
        /* Update status based on control settings */
        if (s->control & CTRL_TX_EN) {
            s->status |= STATUS_TX_READY;
            printf("Custom UART: TX enabled, status now 0x%x\n", s->status);
        } else {
            s->status &= ~STATUS_TX_READY;
            printf("Custom UART: TX disabled, status now 0x%x\n", s->status);
        }
        break;
        
    case UART_STATUS:
        /* Status register is read-only */
        qemu_log_mask(LOG_GUEST_ERROR,
                      "custom-uart: attempt to write to read-only STATUS register\n");
        break;
        
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "custom-uart: invalid write offset 0x%x\n", (int)offset);
        break;
    }
}

static const MemoryRegionOps custom_uart_ops = {
    .read = custom_uart_read,
    .write = custom_uart_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 1,
        .max_access_size = 4,
    },
};

static void custom_uart_receive(void *opaque, const uint8_t *buf, int size)
{
    CustomUartState *s = CUSTOM_UART(opaque);
    
    if (!(s->control & CTRL_RX_EN)) {
        return;
    }
    
    for (int i = 0; i < size && s->rx_count < sizeof(s->rx_fifo); i++) {
        s->rx_fifo[s->rx_count++] = buf[i];
    }
    
    if (s->rx_count > 0) {
        s->status |= STATUS_RX_AVAIL;
        
        /* Generate interrupt if enabled */
        if (s->control & CTRL_RX_INT_EN) {
            qemu_irq_raise(s->irq);
        }
    }
}

static int custom_uart_can_receive(void *opaque)
{
    CustomUartState *s = CUSTOM_UART(opaque);
    
    if (!(s->control & CTRL_RX_EN)) {
        return 0;
    }
    
    return sizeof(s->rx_fifo) - s->rx_count;
}

static void custom_uart_reset(DeviceState *dev)
{
    CustomUartState *s = CUSTOM_UART(dev);
    
    s->data = 0;
    s->status = STATUS_TX_READY;  /* Initially ready to transmit */
    s->control = 0;
    s->rx_count = 0;
    
    qemu_irq_lower(s->irq);
    
    /* Debug output */
    printf("Custom UART reset: status=0x%x, control=0x%x\n", s->status, s->control);
}

static void custom_uart_realize(DeviceState *dev, Error **errp)
{
    CustomUartState *s = CUSTOM_UART(dev);
    
    qemu_chr_fe_set_handlers(&s->chr, custom_uart_can_receive, custom_uart_receive,
                            NULL, NULL, s, NULL, true);
}

static void custom_uart_init(Object *obj)
{
    CustomUartState *s = CUSTOM_UART(obj);
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    
    /* Create memory region */
    memory_region_init_io(&s->mmio, obj, &custom_uart_ops, s,
                          TYPE_CUSTOM_UART, 0x100);
    sysbus_init_mmio(sbd, &s->mmio);
    
    /* Create IRQ */
    sysbus_init_irq(sbd, &s->irq);
}

static Property custom_uart_properties[] = {
    DEFINE_PROP_CHR("chardev", CustomUartState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static const VMStateDescription vmstate_custom_uart = {
    .name = TYPE_CUSTOM_UART,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT8(data, CustomUartState),
        VMSTATE_UINT32(status, CustomUartState),
        VMSTATE_UINT32(control, CustomUartState),
        VMSTATE_UINT8_ARRAY(rx_fifo, CustomUartState, 16),
        VMSTATE_INT32(rx_count, CustomUartState),
        VMSTATE_END_OF_LIST()
    }
};

static void custom_uart_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    
    dc->reset = custom_uart_reset;
    dc->realize = custom_uart_realize;
    dc->props = custom_uart_properties;
    dc->vmsd = &vmstate_custom_uart;
}

static const TypeInfo custom_uart_info = {
    .name = TYPE_CUSTOM_UART,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(CustomUartState),
    .instance_init = custom_uart_init,
    .class_init = custom_uart_class_init,
};

static void custom_uart_register_types(void)
{
    type_register_static(&custom_uart_info);
}

type_init(custom_uart_register_types)
