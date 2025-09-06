# QEMU ARM Development Environment

This repository provides a QEMU development environment for ARM Cortex-M0 bare metal development, including QEMU source code, custom peripheral implementations, and example projects.

## 🚀 Quick Start (自动化设置)

**推荐使用我们的自动化设置脚本，一键完成所有配置：**

```bash
# 克隆项目
git clone <your-repo-url>
cd qemu_env

# 运行自动化设置脚本 (完整流程)
./setup.sh

# 或者分步执行：
./setup.sh --deps      # 仅安装依赖
./setup.sh --source    # 仅下载QEMU源码
./setup.sh --integrate # 仅集成自定义设备
./setup.sh --build     # 仅编译QEMU
./setup.sh --test      # 仅测试自定义设备
```

**脚本将自动处理：**
- ✅ 安装所有必要的依赖包
- ✅ 下载QEMU源码 (通过git submodule或直接克隆)
- ✅ 集成我们的自定义UART设备到QEMU源码
- ✅ 编译带有自定义设备的QEMU
- ✅ 测试自定义设备功能

## Overview

The project includes:
- QEMU source code as a git submodule
- **Custom UART peripheral implementation** with full register interface
- Custom machine type (`microbit-custom`) with enhanced peripherals
- ARM Cortex-M0 bare metal examples with UART communication
- Complete build system for cross-compilation
- Support for debugging with GDB and semihosting
- **Automated setup script** for easy deployment
- CI/CD pipeline for automated testing

## 🚀 Key Features

- **Custom UART Peripheral**: Full implementation at memory address `0x60000000`
- **Hardware Development**: Example of how to add custom peripherals to QEMU
- **Professional Development Tools**: Complete debugging and testing infrastructure
- **One-Click Setup**: Automated script handles all configuration steps

## Custom UART Peripheral

This project includes a **custom UART peripheral** implementation that demonstrates how to add hardware peripherals to QEMU.

### UART Specifications

- **Memory Address**: `0x60000000`
- **Register Interface**:
  - `STATUS` (0x00): TX_READY flag (bit 0)
  - `CONTROL` (0x04): Enable bits (TX_EN=1, RX_EN=2)  
  - `DATA` (0x08): Character transmission register
- **Character Backend**: Supports file output and serial redirection

### Testing the Custom UART

```bash
# Build and test the custom UART example
cd examples/m0
make test-custom-uart

# Expected output:
# "Hello from Custom UART!"
# "UART is working properly!"
```

### Using Custom UART in Your Code

```c
#define CUSTOM_UART_BASE 0x60000000
#define UART_STATUS  (*(volatile uint32_t*)(CUSTOM_UART_BASE + 0x00))
#define UART_CONTROL (*(volatile uint32_t*)(CUSTOM_UART_BASE + 0x04))
#define UART_DATA    (*(volatile uint32_t*)(CUSTOM_UART_BASE + 0x08))

// Initialize UART
UART_CONTROL = 0x3; // Enable TX and RX

// Send character
if (UART_STATUS & 0x1) { // Check TX_READY
    UART_DATA = 'A';
}
```

## Quick Setup (Recommended)

**使用自动化脚本 (推荐方式):**

```bash
./setup.sh --all    # 完整设置流程
```

**手动设置 (如果需要自定义):**

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

**注意**: 如果你使用了我们的自动化脚本 `./setup.sh`，可以跳过这个部分。脚本已经自动处理了QEMU源码下载、自定义设备集成和编译。

If you need to build QEMU manually from the included submodule:

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

### Configure and Build QEMU with Custom Peripherals

```bash
# Navigate to QEMU directory
cd qemu

# Create build directory and configure for ARM support with debug enabled
mkdir -p build
cd build
../configure --target-list=arm-softmmu --enable-debug

# Build QEMU (this takes 15-30 minutes)
make -j$(nproc)

# Verify the build
./arm-softmmu/qemu-system-arm --version
```

The built QEMU binary will be located at `qemu/build/arm-softmmu/qemu-system-arm`.

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

### Using System QEMU (Basic Examples)

```bash
cd examples/m0
make QEMU_PATH=qemu-system-arm run
```

### Using Custom QEMU with Enhanced Features

```bash
cd examples/m0

# Run basic example
make run

# Test custom UART peripheral
make test-custom-uart

# Run with UART output to file
make test-custom-uart-file
```

Expected outputs:
- **Basic example**: `Hello from M0`
- **Custom UART**: `Hello from Custom UART!` and `UART is working properly!`

### Manual Execution with Custom Machine Type

```bash
# Using built QEMU with custom machine type
qemu/build/arm-softmmu/qemu-system-arm \
    -M microbit-custom \
    -nographic \
    -semihosting \
    -serial null \
    -serial file:uart_output.txt \
    -kernel examples/m0/build/test_custom_uart.elf

# View UART output
cat uart_output.txt
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
├── setup.sh                         # 🔧 Automated setup script
├── custom-devices/                  # 🔌 Custom device source files
│   ├── custom-uart.c                # Custom UART peripheral implementation
│   ├── custom-uart.h                # Custom UART header file
│   └── microbit-custom.c            # Custom machine type definition
├── examples/m0/                     # Cortex-M0 bare metal examples
│   ├── Makefile                     # Build system with custom UART support
│   ├── README.md                    # Example-specific documentation
│   ├── src/main.c                   # Main program
│   ├── src/semihosting.c/.h         # Console I/O support
│   ├── src/test_custom_uart.c       # Custom UART test program
│   ├── startup/startup_m0.S         # ARM startup code
│   └── linker/m0.ld                 # Memory layout
├── qemu/                            # QEMU source (submodule) with custom peripherals
│   ├── hw/char/custom-uart.c        # ⚡ Integrated by setup.sh
│   ├── hw/char/custom-uart.h        # ⚡ Integrated by setup.sh
│   ├── hw/arm/microbit-custom.c     # ⚡ Integrated by setup.sh
│   └── ...                          # Standard QEMU source
└── .github/workflows/setup_env.yml  # CI/CD pipeline
```

## Automated Setup Script

### Script Features

The `setup.sh` script provides the following functionality:

- **Dependency Installation**: Automatically installs all required packages
- **Source Management**: Downloads QEMU source code via submodule or direct clone
- **Device Integration**: Copies custom device files to correct QEMU locations
- **Build Configuration**: Updates Makefile.objs files automatically
- **Compilation**: Builds QEMU with custom devices enabled
- **Testing**: Validates custom device functionality

### Script Usage

```bash
./setup.sh [option]

Options:
  -h, --help          Show help message
  -d, --deps          Install dependencies only  
  -s, --source        Download QEMU source only
  -i, --integrate     Integrate custom devices only
  -b, --build         Build QEMU only
  -t, --test          Test custom devices only
  -a, --all           Full setup process (default)
```

### Script Requirements

- Ubuntu/Debian Linux system (recommended)
- Internet connection for downloading packages and source code
- Sudo privileges for package installation
- At least 2GB free disk space for QEMU source and build

## Hardware Development

### Custom Peripheral Architecture

The custom UART demonstrates professional hardware peripheral development:

1. **Memory-Mapped Registers**: Standard register interface at `0x60000000`
2. **Status and Control Logic**: Proper flag management and enable controls
3. **Character Backend Integration**: Connects to QEMU's character device system
4. **Debug Interface**: Comprehensive logging for hardware debugging
5. **Machine Integration**: Properly integrated into custom machine type

### Key Implementation Files

- **`qemu/hw/char/custom-uart.c`**: Complete UART peripheral implementation
- **`qemu/hw/arm/microbit-custom.c`**: Machine type with UART integration
- **`examples/m0/src/test_custom_uart.c`**: Hardware validation test

## Available Make Targets

In the `examples/m0/` directory:

```bash
make                      # Build all (ELF, BIN, HEX)
make clean               # Remove build artifacts  
make size                # Show memory usage
make run                 # Execute basic example on QEMU
make debug               # Start QEMU with GDB server
make help                # Show all targets

# Custom UART targets
make test-custom-uart    # Build and run UART test
make test-custom-uart-file  # Run UART test with file output
make build-custom-uart   # Build UART test program only
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
- **Custom UART**: 0x60000000 - 0x60000FFF (4K)
  - STATUS register (0x60000000)
  - CONTROL register (0x60000004)  
  - DATA register (0x60000008)

## Contributing

This project follows standard ARM bare metal development practices. When contributing:

1. **Code Quality**: Ensure code builds without warnings
2. **Testing**: Test on both system and built QEMU versions  
3. **Documentation**: Update documentation for any new features
4. **Style**: Follow the existing code style
5. **Hardware Development**: For custom peripherals:
   - Follow QEMU device model conventions
   - Add comprehensive register documentation
   - Include validation test programs
   - Ensure proper memory mapping

## Development Guidelines

### Adding New Peripherals

1. Create peripheral implementation in `qemu/hw/[category]/`
2. Add to appropriate `Makefile.objs`
3. Integrate into machine definition
4. Create test program in `examples/`
5. Update documentation

### Testing Custom Hardware

```bash
# Full validation sequence
make clean
cd qemu/build && make -j$(nproc)
cd ../../examples/m0
make test-custom-uart
```

## License

This project includes QEMU as a submodule, which is licensed under various open source licenses. See the QEMU project for details.
