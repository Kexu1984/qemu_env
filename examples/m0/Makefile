# Makefile for ARM Cortex-M0 bare metal example

# Project name
PROJECT = m0

# Tools
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc
LD = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump
SIZE = $(PREFIX)size
GDB = $(PREFIX)gdb

# Directories
SRCDIR = src
STARTUPDIR = startup
LINKERDIR = linker
BUILDDIR = build

# Source files
C_SOURCES = $(wildcard $(SRCDIR)/*.c)
ASM_SOURCES = $(wildcard $(STARTUPDIR)/*.S)

# Object files
C_OBJECTS = $(C_SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
ASM_OBJECTS = $(ASM_SOURCES:$(STARTUPDIR)/%.S=$(BUILDDIR)/%.o)
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Compiler flags
CFLAGS = -mcpu=cortex-m0 \
         -mthumb \
         -Os \
         -Wall \
         -Wextra \
         -Wno-unused-parameter \
         -ffreestanding \
         -nostdlib \
         -fdata-sections \
         -ffunction-sections \
         -I$(SRCDIR)

# Assembler flags
ASFLAGS = -mcpu=cortex-m0 \
          -mthumb \
          -Wall \
          -fdata-sections \
          -ffunction-sections

# Linker flags
LDFLAGS = -mcpu=cortex-m0 \
          -mthumb \
          -T$(LINKERDIR)/$(PROJECT).ld \
          -Wl,--gc-sections \
          -Wl,-Map,$(BUILDDIR)/$(PROJECT).map \
          -nostdlib \
          -nostartfiles

# QEMU settings
QEMU_MACHINE = microbit
QEMU_CPU = cortex-m0
QEMU_EXTRA_FLAGS = -nographic -semihosting

# Default target
all: $(BUILDDIR)/$(PROJECT).elf $(BUILDDIR)/$(PROJECT).bin $(BUILDDIR)/$(PROJECT).hex

# Create build directory
$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

# Compile C source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM source files
$(BUILDDIR)/%.o: $(STARTUPDIR)/%.S | $(BUILDDIR)
	@echo "AS $<"
	@$(AS) $(ASFLAGS) -c $< -o $@

# Link ELF file
$(BUILDDIR)/$(PROJECT).elf: $(OBJECTS)
	@echo "LD $@"
	@$(LD) $(LDFLAGS) $(OBJECTS) -o $@
	@echo "Size information:"
	@$(SIZE) $@

# Generate binary file
$(BUILDDIR)/$(PROJECT).bin: $(BUILDDIR)/$(PROJECT).elf
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O binary $< $@

# Generate hex file
$(BUILDDIR)/$(PROJECT).hex: $(BUILDDIR)/$(PROJECT).elf
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O ihex $< $@

# Generate disassembly
$(BUILDDIR)/$(PROJECT).lst: $(BUILDDIR)/$(PROJECT).elf
	@echo "OBJDUMP $@"
	@$(OBJDUMP) -h -S $< > $@

# Show size information
size: $(BUILDDIR)/$(PROJECT).elf
	@$(SIZE) $<

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILDDIR)

# QEMU targets
QEMU_PATH ?= ../../qemu/build/qemu-system-arm
CUSTOM_QEMU_PATH ?= /home/xuke/qemu_env/qemu/build/arm-softmmu/qemu-system-arm

run: $(BUILDDIR)/$(PROJECT).elf
	@echo "Running $(PROJECT).elf in QEMU..."
	@$(QEMU_PATH) -M $(QEMU_MACHINE) $(QEMU_EXTRA_FLAGS) -kernel $<

debug: $(BUILDDIR)/$(PROJECT).elf
	@echo "Starting QEMU with GDB server..."
	@$(QEMU_PATH) -M $(QEMU_MACHINE) $(QEMU_EXTRA_FLAGS) -kernel $< -S -gdb tcp::1234 &
	@echo "Connect with: $(GDB) $<"
	@echo "Then use: target remote :1234"

# Custom UART test
test-custom-uart: $(BUILDDIR)/test_custom_uart.elf
	@echo "Running custom UART test..."
	@$(CUSTOM_QEMU_PATH) -M microbit-custom -nographic -semihosting -kernel $<

$(BUILDDIR)/test_custom_uart.elf: $(BUILDDIR)/test_custom_uart.o $(BUILDDIR)/semihosting.o $(BUILDDIR)/startup_m0.o
	@echo "LD $@"
	@$(LD) $(LDFLAGS) $^ -o $@
	@echo "Size information:"
	@$(SIZE) $@

$(BUILDDIR)/test_custom_uart.o: $(SRCDIR)/test_custom_uart.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Help target
help:
	@echo "Available targets:"
	@echo "  all               - Build ELF, BIN, and HEX files (default)"
	@echo "  clean             - Remove build artifacts"
	@echo "  size              - Show size information"
	@echo "  run               - Run in QEMU"
	@echo "  debug             - Start QEMU with GDB server"
	@echo "  test-custom-uart  - Test custom UART peripheral"
	@echo "  help              - Show this help message"
	@echo ""
	@echo "Build artifacts are placed in $(BUILDDIR)/"

# Phony targets
.PHONY: all clean size run debug help test-custom-uart

# Dependencies
-include $(OBJECTS:.o=.d)