# QEMU ARM Development Environment

This repository provides a QEMU development environment for ARM Cortex-M0 bare metal development, including QEMU source code and example projects.

## Overview

The project includes:
- QEMU source code as a git submodule
- ARM Cortex-M0 bare metal examples 
- Complete build system for cross-compilation
- Support for debugging with GDB and semihosting
- CI/CD pipeline for automated testing

## Quick Setup (Recommended)

For fastest setup, use system packages:

```bash
# Update package lists
sudo apt-get update

# Install QEMU ARM emulator  
sudo apt-get install -y qemu-system-arm

# Install ARM cross-compilation toolchain
sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi \
    libnewlib-arm-none-eabi gdb-multiarch

# Initialize git submodules
git submodule update --init
```

## Building QEMU from Source (Advanced)

If you need to build QEMU from the included submodule:

### Prerequisites

Install build dependencies:

```bash
sudo apt-get update
sudo apt-get install -y git ninja-build pkg-config python3 meson build-essential \
    libglib2.0-dev libpixman-1-dev zlib1g-dev libfdt-dev
```

### Initialize Submodule

```bash
# Initialize the QEMU submodule (if not already done)
git submodule update --init
```

### Configure and Build QEMU

```bash
# Navigate to QEMU directory
cd qemu

# Create build directory and configure for ARM support
mkdir -p build
cd build
../configure --target-list=arm-softmmu

# Build QEMU (this takes 15-30 minutes)
make -j$(nproc)

# Verify the build
./qemu-system-arm --version
```

The built QEMU binary will be located at `qemu/build/qemu-system-arm`.

## Building ARM Examples

### Install ARM Toolchain

If not already installed:

```bash
sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi \
    libnewlib-arm-none-eabi
```

### Build the Cortex-M0 Example

```bash
cd examples/m0

# Clean and build
make clean
make

# Check build results
file build/m0.elf
arm-none-eabi-size build/m0.elf
```

## Running Examples

### Using System QEMU

```bash
cd examples/m0
make QEMU_PATH=qemu-system-arm run
```

### Using Built QEMU

```bash
cd examples/m0
make run
```

Expected output:
```
Hello from M0
```

### Manual Execution

You can also run manually:

```bash
# Using system QEMU
qemu-system-arm -M microbit -nographic -semihosting -kernel examples/m0/build/m0.elf

# Using built QEMU  
qemu/build/qemu-system-arm -M microbit -nographic -semihosting -kernel examples/m0/build/m0.elf
```

## Debugging with GDB

### Start QEMU with GDB Server

```bash
cd examples/m0

# Using system QEMU
make QEMU_PATH=qemu-system-arm debug

# Using built QEMU
make debug
```

### Connect with GDB

In another terminal:

```bash
cd examples/m0
arm-none-eabi-gdb build/m0.elf
```

In GDB:
```
(gdb) target remote :1234
(gdb) continue
```

## Project Structure

```
.
├── README.md                        # This file
├── examples/m0/                     # Cortex-M0 bare metal example
│   ├── Makefile                     # Build system
│   ├── README.md                    # Example-specific documentation
│   ├── src/main.c                   # Main program
│   ├── src/semihosting.c/.h         # Console I/O support
│   ├── startup/startup_m0.S         # ARM startup code
│   └── linker/m0.ld                 # Memory layout
├── qemu/                            # QEMU source (submodule)
└── .github/workflows/setup_env.yml  # CI/CD pipeline
```

## Available Make Targets

In the `examples/m0/` directory:

```bash
make          # Build all (ELF, BIN, HEX)
make clean    # Remove build artifacts  
make size     # Show memory usage
make run      # Execute on QEMU
make debug    # Start QEMU with GDB server
make help     # Show all targets
```

## Troubleshooting

### Common Issues

1. **"No qemu-system-arm found"**: Install system package: `sudo apt-get install qemu-system-arm`
2. **"arm-none-eabi-gcc not found"**: Install toolchain: `sudo apt-get install gcc-arm-none-eabi`  
3. **Build takes too long**: Use system packages instead of building QEMU from source
4. **QEMU hangs**: Use timeout wrapper: `timeout 10 qemu-system-arm ...`

### Memory Layout

The Cortex-M0 example uses this memory layout:

- **FLASH**: 0x00000000 - 0x0003FFFF (256K)
  - Vector table at 0x00000000
  - Program code and constants
- **RAM**: 0x20000000 - 0x20003FFF (16K)
  - Data section (initialized variables)
  - BSS section (uninitialized variables)  
  - Stack (1KB)
  - Heap (remaining RAM)

## Contributing

This project follows standard ARM bare metal development practices. When contributing:

1. Ensure code builds without warnings
2. Test on both system and built QEMU versions
3. Update documentation for any new features
4. Follow the existing code style

## License

This project includes QEMU as a submodule, which is licensed under various open source licenses. See the QEMU project for details.
