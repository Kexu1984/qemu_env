#include "semihosting.h"

/**
 * ARM semihosting system calls
 */
#define SYS_WRITE0  0x04
#define SYS_EXIT    0x18

/**
 * @brief Execute semihosting call using BKPT instruction
 * @param sysnum System call number
 * @param arg Pointer to arguments (or argument value for simple calls)
 * @return Return value from semihosting call
 */
static inline int semihost_call(int sysnum, void* arg)
{
    int result;
    
    __asm__ volatile (
        "mov r0, %[sysnum]\n\t"     /* System call number in r0 */
        "mov r1, %[arg]\n\t"        /* Arguments in r1 */
        "bkpt #0xAB\n\t"            /* ARM semihosting breakpoint */
        "mov %[result], r0\n\t"     /* Return value from r0 */
        : [result] "=r" (result)
        : [sysnum] "r" (sysnum), [arg] "r" (arg)
        : "r0", "r1", "memory"
    );
    
    return result;
}

void sh_write0(const char *s)
{
    semihost_call(SYS_WRITE0, (void*)s);
}

void sh_exit(int code)
{
    semihost_call(SYS_EXIT, (void*)code);
    
    /* Should never reach here, but just in case */
    while (1) {
        __asm__ volatile ("wfi"); /* Wait for interrupt */
    }
}