# GDB configuration for ARM Cortex-M0 debugging
set architecture arm
set endian little

# Connect to QEMU GDB server automatically when loading this script
# Use: arm-none-eabi-gdb build/m0.elf -x .gdbinit

# Uncomment to automatically connect:
# target remote :1234

# Common debugging commands
define reset
    monitor system_reset
end

define reload
    load
    monitor system_reset
end

# Show vector table
define vectors
    x/16xw 0x00000000
end

# Show stack
define showstack
    x/16xw $sp
end

# Pretty print options
set print pretty on
set print array on

echo \nLoaded ARM Cortex-M0 GDB configuration\n
echo Use 'target remote :1234' to connect to QEMU\n
echo Custom commands: reset, reload, vectors, showstack\n\n