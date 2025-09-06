/*
 * BBC micro:bit machine with custom UART
 * Based on the original microbit.c but adds a custom UART peripheral
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/arm/boot.h"
#include "sysemu/sysemu.h"
#include "exec/address-spaces.h"

#include "hw/arm/nrf51_soc.h"
#include "hw/i2c/microbit_i2c.h"
#include "hw/qdev-properties.h"

#define TYPE_CUSTOM_UART "custom-uart"

typedef struct {
    MachineState parent;
    NRF51State nrf51;
    MicrobitI2CState i2c;
    DeviceState *custom_uart;
} MicrobitCustomMachineState;

#define TYPE_MICROBIT_CUSTOM_MACHINE MACHINE_TYPE_NAME("microbit-custom")

#define MICROBIT_CUSTOM_MACHINE(obj) \
    OBJECT_CHECK(MicrobitCustomMachineState, obj, TYPE_MICROBIT_CUSTOM_MACHINE)

/* Custom UART will be mapped to this address */
#define CUSTOM_UART_BASE    0x60000000

static void microbit_custom_init(MachineState *machine)
{
    MicrobitCustomMachineState *s = MICROBIT_CUSTOM_MACHINE(machine);
    MemoryRegion *system_memory = get_system_memory();
    MemoryRegion *mr;
    Object *soc = OBJECT(&s->nrf51);
    Object *i2c = OBJECT(&s->i2c);

    /* Initialize the nRF51 SoC */
    sysbus_init_child_obj(OBJECT(machine), "nrf51", soc, sizeof(s->nrf51),
                          TYPE_NRF51_SOC);
    qdev_prop_set_chr(DEVICE(&s->nrf51), "serial0", serial_hd(0));
    object_property_set_link(soc, OBJECT(system_memory), "memory",
                             &error_fatal);
    object_property_set_bool(soc, true, "realized", &error_fatal);

    /* Add the original I2C device */
    sysbus_init_child_obj(OBJECT(machine), "microbit.twi", i2c,
                          sizeof(s->i2c), TYPE_MICROBIT_I2C);
    object_property_set_bool(i2c, true, "realized", &error_fatal);
    mr = sysbus_mmio_get_region(SYS_BUS_DEVICE(i2c), 0);
    memory_region_add_subregion_overlap(&s->nrf51.container, NRF51_TWI_BASE,
                                        mr, -1);

    /* Add our custom UART */
    s->custom_uart = qdev_create(NULL, TYPE_CUSTOM_UART);
    
    /* Connect to serial device if available */
    if (serial_hd(1)) {
        qdev_prop_set_chr(s->custom_uart, "chardev", serial_hd(1));
    }
    
    qdev_init_nofail(s->custom_uart);
    
    /* Map the custom UART to memory */
    sysbus_mmio_map(SYS_BUS_DEVICE(s->custom_uart), 0, CUSTOM_UART_BASE);
    
    /* Connect IRQ (optional - you can connect to CPU if needed) */
    /* sysbus_connect_irq(SYS_BUS_DEVICE(s->custom_uart), 0, 
                          qdev_get_gpio_in(DEVICE(&s->nrf51.cpu), 16)); */

    printf("Custom UART mapped to address 0x%08x\n", CUSTOM_UART_BASE);

    /* Load kernel */
    armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename,
                       NRF51_SOC(soc)->flash_size);
}

static void microbit_custom_machine_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "BBC micro:bit with custom UART";
    mc->init = microbit_custom_init;
    mc->max_cpus = 1;
}

static const TypeInfo microbit_custom_info = {
    .name = TYPE_MICROBIT_CUSTOM_MACHINE,
    .parent = TYPE_MACHINE,
    .instance_size = sizeof(MicrobitCustomMachineState),
    .class_init = microbit_custom_machine_class_init,
};

static void microbit_custom_machine_init(void)
{
    type_register_static(&microbit_custom_info);
}

type_init(microbit_custom_machine_init);
