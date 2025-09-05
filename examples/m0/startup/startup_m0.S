/**
 * Startup file for ARM Cortex-M0
 * 
 * This file provides:
 * - Vector table with stack pointer and reset handler
 * - Reset handler that initializes memory and calls main
 * - Default exception handlers
 */

    .syntax unified
    .cpu cortex-m0
    .thumb

    .section .vectors, "a"
    .align 2
    .globl _vectors
    .type _vectors, %object

_vectors:
    .word   _estack                 /* 0x00: Initial Stack Pointer */
    .word   Reset_Handler           /* 0x04: Reset Handler */
    .word   NMI_Handler             /* 0x08: NMI Handler */
    .word   HardFault_Handler       /* 0x0C: Hard Fault Handler */
    .word   0                       /* 0x10: Reserved */
    .word   0                       /* 0x14: Reserved */
    .word   0                       /* 0x18: Reserved */
    .word   0                       /* 0x1C: Reserved */
    .word   0                       /* 0x20: Reserved */
    .word   0                       /* 0x24: Reserved */
    .word   0                       /* 0x28: Reserved */
    .word   SVC_Handler             /* 0x2C: SVCall Handler */
    .word   0                       /* 0x30: Reserved */
    .word   0                       /* 0x34: Reserved */
    .word   PendSV_Handler          /* 0x38: PendSV Handler */
    .word   SysTick_Handler         /* 0x3C: SysTick Handler */

    /* External interrupt vectors (can be extended as needed) */
    .word   DefaultHandler          /* 0x40: IRQ0 */
    .word   DefaultHandler          /* 0x44: IRQ1 */
    .word   DefaultHandler          /* 0x48: IRQ2 */
    .word   DefaultHandler          /* 0x4C: IRQ3 */

    .size _vectors, . - _vectors

    .text
    .thumb
    .thumb_func
    .align 2
    .globl Reset_Handler
    .type Reset_Handler, %function

Reset_Handler:
    /* Copy data from FLASH to RAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
    movs r3, #0
    b LoopCopyDataInit

CopyDataInit:
    ldr r4, [r2, r3]
    str r4, [r0, r3]
    adds r3, r3, #4

LoopCopyDataInit:
    adds r4, r0, r3
    cmp r4, r1
    bcc CopyDataInit

    /* Zero fill the bss segment */
    ldr r2, =_sbss
    ldr r4, =_ebss
    movs r3, #0
    b LoopFillZerobss

FillZerobss:
    str r3, [r2]
    adds r2, r2, #4

LoopFillZerobss:
    cmp r2, r4
    bcc FillZerobss

    /* Call main function */
    bl main

    /* If main returns, loop forever */
    b .

    .size Reset_Handler, . - Reset_Handler

    /* Default handler for all exceptions/interrupts */
    .section .text.DefaultHandler, "ax", %progbits
DefaultHandler:
Infinite_Loop:
    b Infinite_Loop
    .size DefaultHandler, . - DefaultHandler

    /* Weak symbol definitions for exception handlers */
    .weak NMI_Handler
    .thumb_set NMI_Handler,DefaultHandler
    
    .weak HardFault_Handler
    .thumb_set HardFault_Handler,DefaultHandler
    
    .weak SVC_Handler
    .thumb_set SVC_Handler,DefaultHandler
    
    .weak PendSV_Handler
    .thumb_set PendSV_Handler,DefaultHandler
    
    .weak SysTick_Handler
    .thumb_set SysTick_Handler,DefaultHandler

    .end