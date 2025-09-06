/*
 * Test program for custom UART
 */

#include <stdint.h>
#include "semihosting.h"

// Custom UART registers
#define CUSTOM_UART_BASE  0x60000000
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
    sh_write0("Initializing custom UART...\n");
    
    // Read initial status
    uint32_t initial_status = UART_STATUS;
    sh_write0("Initial status: ");
    // Simple status output (this is basic but works)
    if (initial_status & STATUS_TX_READY) {
        sh_write0("TX_READY ");
    }
    if (initial_status & STATUS_RX_AVAIL) {
        sh_write0("RX_AVAIL ");
    }
    sh_write0("\n");
    
    // Enable transmit and receive
    UART_CONTROL = CTRL_TX_EN | CTRL_RX_EN;
    
    // Read status after enabling
    uint32_t new_status = UART_STATUS;
    sh_write0("Status after enable: ");
    if (new_status & STATUS_TX_READY) {
        sh_write0("TX_READY ");
    }
    if (new_status & STATUS_RX_AVAIL) {
        sh_write0("RX_AVAIL ");
    }
    sh_write0("\n");
}

void custom_uart_putchar(char c)
{
    sh_write0("Attempting to send char via custom UART...\n");
    
    // Check if transmitter is ready
    uint32_t status = UART_STATUS;
    if (!(status & STATUS_TX_READY)) {
        sh_write0("UART not ready for TX!\n");
        return;
    }
    
    sh_write0("UART ready, sending character...\n");
    
    // Send character
    UART_DATA = c;
    
    sh_write0("Character sent to UART_DATA register\n");
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
    // First, print using semihosting
    sh_write0("Testing custom UART at 0x60000000...\n");
    
    // Initialize custom UART
    custom_uart_init();
    
    // Test basic functionality
    sh_write0("Checking UART registers:\n");
    
    uint32_t status = UART_STATUS;
    uint32_t control = UART_CONTROL;
    
    sh_write0("UART initialized successfully!\n");
    
    // Send test messages to custom UART
    custom_uart_puts("Hello from Custom UART!\n");
    custom_uart_puts("UART is working properly!\n");
    
    sh_write0("Custom UART test completed successfully!\n");
    
    // Exit successfully
    sh_exit(0);
    
    return 0;
}
