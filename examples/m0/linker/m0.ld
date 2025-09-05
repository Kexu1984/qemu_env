/**
 * Linker script for ARM Cortex-M0
 * 
 * Memory layout based on typical Cortex-M0 microcontroller:
 * - FLASH: 0x00000000 - 0x0003FFFF (256K)
 * - RAM:   0x20000000 - 0x20003FFF (16K)
 */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 256K
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 16K
}

/* Stack size */
_stack_size = 0x400; /* 1KB stack */

/* Entry point */
ENTRY(Reset_Handler)

SECTIONS
{
    /* Vector table and code in FLASH */
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.vectors))           /* Vector table */
        . = ALIGN(4);
        *(.text)                    /* Program code */
        *(.text*)
        *(.rodata)                  /* Read-only data */
        *(.rodata*)
        
        /* C++ constructors/destructors (if needed) */
        KEEP(*(.init))
        KEEP(*(.fini))
        
        . = ALIGN(4);
        _etext = .;                 /* End of .text section */
    } > FLASH
    
    /* ARM exception unwinding (if needed) */
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    
    /* Initialized data section */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;                 /* Start of .data section */
        *(.data)                    /* Initialized data */
        *(.data*)
        . = ALIGN(4);
        _edata = .;                 /* End of .data section */
    } > RAM AT > FLASH
    
    _sidata = LOADADDR(.data);      /* LMA of .data section in FLASH */
    
    /* Uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;                  /* Start of .bss section */
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;                  /* End of .bss section */
    } > RAM
    
    /* Stack section */
    .stack :
    {
        . = ALIGN(8);
        _sstack = .;                /* Start of stack */
        . = . + _stack_size;
        . = ALIGN(8);
        _estack = .;                /* End of stack (initial SP) */
    } > RAM
    
    /* Heap section (remaining RAM) */
    .heap :
    {
        . = ALIGN(4);
        _sheap = .;                 /* Start of heap */
        . = ORIGIN(RAM) + LENGTH(RAM);
        _eheap = .;                 /* End of heap */
    } > RAM
    
    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }
    
    /* Debug information */
    .ARM.attributes 0 : { *(.ARM.attributes) }
}