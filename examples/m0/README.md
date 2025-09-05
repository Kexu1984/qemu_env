# ARM Cortex-M0 Bare Metal Example

This is a minimal bare metal example for ARM Cortex-M0 that demonstrates:

- Basic startup code with vector table
- Semihosting support for console output and clean exit
- Linker script for typical Cortex-M0 memory layout
- Makefile-based build system

## Prerequisites

Install the ARM cross-compilation toolchain:

```bash
sudo apt-get update
sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi
```

## Building

Build all targets (ELF, BIN, HEX):

```bash
make
```

Show size information:

```bash
make size
```

Clean build artifacts:

```bash
make clean
```

## Running

Run in QEMU (requires QEMU with ARM support):

```bash
make run
# Or manually:
../../qemu/build/qemu-system-arm -M microbit -nographic -semihosting -kernel build/m0.elf
```

Expected output:
```
Hello from M0
```

## Debugging

Start QEMU with GDB server:

```bash
make debug
```

Then in another terminal:

```bash
arm-none-eabi-gdb build/m0.elf
(gdb) target remote :1234
(gdb) continue
```

## Memory Layout

- **FLASH**: 0x00000000 - 0x0003FFFF (256K)
  - Vector table at 0x00000000
  - Program code and constants
- **RAM**: 0x20000000 - 0x20003FFF (16K)
  - Data section (initialized variables)
  - BSS section (uninitialized variables)
  - Stack (1KB)
  - Heap (remaining RAM)

## Files

- `src/main.c` - Main program
- `src/semihosting.c/.h` - Semihosting wrapper functions
- `startup/startup_m0.S` - Startup code and vector table
- `linker/m0.ld` - Linker script
- `Makefile` - Build system