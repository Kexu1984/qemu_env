#ifndef HW_CHAR_CUSTOM_UART_H
#define HW_CHAR_CUSTOM_UART_H

#include "hw/sysbus.h"
#include "chardev/char-fe.h"

#define TYPE_CUSTOM_UART "custom-uart"
#define CUSTOM_UART(obj) OBJECT_CHECK(CustomUartState, (obj), TYPE_CUSTOM_UART)

/* Custom UART Register Definitions */
#define CUSTOM_UART_STATUS   0x00  /* Status register */
#define CUSTOM_UART_CONTROL  0x04  /* Control register */
#define CUSTOM_UART_DATA     0x08  /* Data register */

/* Status register bits */
#define STATUS_TX_READY      (1 << 0)
#define STATUS_RX_READY      (1 << 1)

/* Control register bits */
#define CONTROL_TX_EN        (1 << 0)
#define CONTROL_RX_EN        (1 << 1)

typedef struct {
    SysBusDevice parent_obj;
    
    MemoryRegion mmio;
    CharBackend chr;
    
    uint32_t status;
    uint32_t control;
    uint32_t data;
} CustomUartState;

#endif /* HW_CHAR_CUSTOM_UART_H */
