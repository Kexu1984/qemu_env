# QEMU ARM Development Environment

This repository provides a QEMU development environment for ARM Cortex-M0 bare metal development, including QEMU source code and example projects.

**ALWAYS follow these instructions first and only search for additional information if these instructions are incomplete or incorrect.**

## Working Effectively

### Quick Bootstrap (Recommended - 2 minutes total)
Use system packages for reliable setup:
```bash
# Update package lists - takes ~30 seconds
sudo apt-get update

# Install QEMU ARM emulator - takes ~30 seconds  
sudo apt-get install -y qemu-system-arm

# Install ARM cross-compilation toolchain - takes ~40 seconds
sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi \
    libnewlib-arm-none-eabi gdb-multiarch

# Initialize git submodules - takes ~10 seconds
git submodule update --init
```

### Alternative: Build QEMU from Source (Advanced - Network Dependent)
**WARNING**: Building QEMU from source requires network access to gitlab.com submodules and **takes 30+ minutes**.

```bash
# ONLY if you need to build QEMU from source and have full network access:
sudo apt-get update
sudo apt-get install -y git ninja-build pkg-config python3 meson build-essential \
    libglib2.0-dev libpixman-1-dev zlib1g-dev libfdt-dev

# Initialize all submodules - may fail with network restrictions
git submodule update --init --recursive

# Configure QEMU - takes ~2 minutes
cd qemu
mkdir -p build
cd build
../configure --target-list=arm-softmmu

# Build QEMU - takes 30-45 minutes. NEVER CANCEL. Set timeout to 60+ minutes.
make -j$(nproc)
```

**KNOWN ISSUE**: QEMU source build fails in network-restricted environments due to gitlab.com submodule dependencies. Use system packages instead.

## Build and Test ARM Examples

### Build the Cortex-M0 Example
```bash
cd examples/m0

# Clean and build - takes <1 second
make clean
make

# Check build artifacts
file build/m0.elf
arm-none-eabi-size build/m0.elf
```

### Run on QEMU
```bash
# Run the example (using system QEMU)
make QEMU_PATH=qemu-system-arm run

# OR manually:
qemu-system-arm -M microbit -nographic -semihosting -kernel build/m0.elf

# Expected output:
# Hello from M0
```

### Debug with GDB
```bash
# Terminal 1: Start QEMU with GDB server
make QEMU_PATH=qemu-system-arm debug
# OR manually:
qemu-system-arm -M microbit -nographic -semihosting -kernel build/m0.elf -S -gdb tcp::1234

# Terminal 2: Connect with GDB
arm-none-eabi-gdb build/m0.elf
# In GDB:
# (gdb) target remote :1234
# (gdb) continue
```

## Validation

**ALWAYS run these validation steps after making changes:**

### Complete End-to-End Validation
1. **Build Test**: 
   ```bash
   cd examples/m0
   make clean && make
   ```
   - Should complete in <1 second
   - Should produce `build/m0.elf`, `build/m0.bin`, `build/m0.hex`

2. **Execution Test**:
   ```bash
   timeout 10 qemu-system-arm -M microbit -nographic -semihosting -kernel build/m0.elf
   ```
   - **CRITICAL**: Must output exactly "Hello from M0"
   - Should exit cleanly after message

3. **Debug Test**:
   ```bash
   # Start QEMU with GDB in background
   qemu-system-arm -M microbit -nographic -semihosting -kernel build/m0.elf -S -gdb tcp::1234 &
   QEMU_PID=$!
   
   # Verify GDB server is listening
   netstat -ln | grep :1234
   
   # Cleanup
   kill $QEMU_PID
   ```
   - Should show GDB server listening on port 1234

### File Verification
```bash
# Check ARM binary format
file build/m0.elf
# Expected: "ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV)"

# Check size (typical values)
arm-none-eabi-size build/m0.elf
# Expected: text ~200 bytes, data 0, bss ~16K
```

## Common Tasks

### Repository Structure
```
.
├── .github/workflows/setup_env.yml  # CI/CD pipeline
├── README.md                        # Basic project info
├── examples/m0/                     # Cortex-M0 bare metal example
│   ├── Makefile                     # Build system
│   ├── README.md                    # Example-specific docs
│   ├── src/main.c                   # Main program
│   ├── src/semihosting.c/.h         # Console I/O support
│   ├── startup/startup_m0.S         # ARM startup code
│   └── linker/m0.ld                 # Memory layout
└── qemu/                            # QEMU source (submodule)
```

### Key Files to Monitor
- **examples/m0/src/main.c**: Main application logic
- **examples/m0/Makefile**: Build configuration and targets
- **examples/m0/linker/m0.ld**: Memory layout for Cortex-M0
- **.github/workflows/setup_env.yml**: CI/CD configuration

### Build Targets Available
```bash
cd examples/m0
make          # Build all (ELF, BIN, HEX)
make clean    # Remove build artifacts  
make size     # Show memory usage
make run      # Execute on QEMU
make debug    # Start QEMU with GDB server
make help     # Show all targets
```

### Timing Expectations
- **Package installation**: 60-90 seconds total
- **Submodule init**: ~10 seconds  
- **M0 example build**: <1 second
- **QEMU execution**: Instant
- **QEMU source build**: 30-45 minutes (if network allows) - **NEVER CANCEL**

## Troubleshooting

### Common Issues
1. **"No qemu-system-arm found"**: Install system package: `sudo apt-get install qemu-system-arm`
2. **"arm-none-eabi-gcc not found"**: Install toolchain: `sudo apt-get install gcc-arm-none-eabi`  
3. **Build fails with network errors**: Use system packages instead of building QEMU from source
4. **QEMU hangs**: Use timeout wrapper: `timeout 10 qemu-system-arm ...`

### Network Restrictions
In environments with limited network access:
- ✅ System packages work reliably
- ❌ QEMU submodule build may fail (gitlab.com dependencies)
- ✅ Main git submodule (qemu) usually works
- ✅ All example builds work offline

### Build Performance
- M0 example builds are extremely fast (<1 second)
- No need to optimize build times
- QEMU execution is instant
- Use system QEMU for fastest setup