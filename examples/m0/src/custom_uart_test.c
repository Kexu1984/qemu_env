/*
 * Test program for custom UART
 * This program will test the custom UART mapped at 0x50000000
 */

#include <stdint.h>

// Custom UART registers
#define CUSTOM_UART_BASE  0x50000000
#define UART_DATA         (*(volatile uint32_t*)(CUSTOM_UART_BASE + 0x00))
#define UART_STATUS       (*(volatile uint32_t*)(CUSTOM_UART_BASE + 0x04))
#define UART_CONTROL      (*(volatile uint32_t*)(CUSTOM_UART_BASE + 0x08))

// Status register bits
#define STATUS_TX_READY   (1 << 0)
#define STATUS_RX_AVAIL   (1 << 1)

// Control register bits
#define CTRL_TX_EN        (1 << 0)
#define CTRL_RX_EN        (1 << 1)
#define CTRL_TX_INT_EN    (1 << 2)
#define CTRL_RX_INT_EN    (1 << 3)

void custom_uart_init(void)
{
    // Enable transmit and receive
    UART_CONTROL = CTRL_TX_EN | CTRL_RX_EN;
}

void custom_uart_putchar(char c)
{
    // Wait until transmitter is ready
    while (!(UART_STATUS & STATUS_TX_READY)) {
        // Wait
    }
    
    // Send character
    UART_DATA = c;
}

void custom_uart_puts(const char *str)
{
    while (*str) {
        custom_uart_putchar(*str);
        str++;
    }
}

int main(void)
{
    // Initialize custom UART
    custom_uart_init();
    
    // Send test messages
    custom_uart_puts("Hello from Custom UART!\r\n");
    custom_uart_puts("Custom UART is working at address 0x50000000\r\n");
    custom_uart_puts("Status register: ");
    
    // Read and display status
    uint32_t status = UART_STATUS;
    custom_uart_putchar('0');
    custom_uart_putchar('x');
    
    // Simple hex output
    for (int i = 28; i >= 0; i -= 4) {
        uint32_t nibble = (status >> i) & 0xF;
        if (nibble < 10) {
            custom_uart_putchar('0' + nibble);
        } else {
            custom_uart_putchar('A' + nibble - 10);
        }
    }
    
    custom_uart_puts("\r\n");
    custom_uart_puts("Test completed!\r\n");
    
    // Loop forever
    while (1) {
        // You could add code to read from UART here
    }
    
    return 0;
}
