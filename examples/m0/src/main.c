#include "semihosting.h"

/**
 * @brief Main function for Cortex-M0 bare metal example
 * 
 * This function demonstrates basic semihosting functionality by:
 * 1. Printing a hello message to the console
 * 2. Exiting cleanly with code 0
 */
int main(void)
{
    /* Print hello message using semihosting */
    sh_write0("Hello from M0\n");
    
    /* Exit cleanly */
    sh_exit(0);
    
    /* Should never reach here */
    return 0;
}