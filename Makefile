# Makefile for Vibe OS

# Compiler and linker settings
CC = x86_64-elf-gcc
AS = x86_64-elf-as
LD = x86_64-elf-ld
QEMU = qemu-system-i386

# Compiler flags
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -nostdlib -nostdinc
ASFLAGS = --32
LDFLAGS = -m elf_i386 -n -T linker.ld -nostdlib

# Source files
C_SOURCES = $(wildcard *.c)
ASM_SOURCES = boot.s
OBJECTS = $(ASM_SOURCES:.s=.o) $(C_SOURCES:.c=.o)

# Output files
KERNEL = kernel.bin

# Default target
all: $(KERNEL)
	@echo "Kernel built successfully. Use './create-iso.sh' to create a bootable ISO."

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble assembly files
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Link the kernel
$(KERNEL): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

# Run kernel directly in QEMU (without ISO)
run-kernel: $(KERNEL)
	$(QEMU) -kernel $(KERNEL) -vga std -no-reboot -no-shutdown -device isa-debug-exit

# Run kernel with curses interface (better for keyboard input)
run-kernel-curses: $(KERNEL)
	$(QEMU) -kernel $(KERNEL) -display curses -no-reboot -no-shutdown

# Run in QEMU with debug options
debug: $(KERNEL)
	$(QEMU) -kernel $(KERNEL) -serial stdio -d int -no-reboot

# Clean build files
clean:
	rm -f *.o $(KERNEL)
	rm -rf isodir
	rm -f vibe-os.iso

.PHONY: all clean run-kernel run-kernel-curses debug 