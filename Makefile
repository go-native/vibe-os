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
ISO = vibe-os.iso

# Default target
all: $(KERNEL)
	@echo "Kernel built successfully. Use 'make iso' to create an ISO image."

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble assembly files
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Link the kernel
$(KERNEL): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

# Create ISO image (requires grub-mkrescue which may not be available on macOS)
iso: $(KERNEL) grub.cfg
	@mkdir -p isodir/boot/grub
	@cp $(KERNEL) isodir/boot/
	@cp grub.cfg isodir/boot/grub/
	@echo "Attempting to create ISO with grub-mkrescue..."
	@if command -v grub-mkrescue >/dev/null 2>&1; then \
		grub-mkrescue -o $(ISO) isodir; \
	else \
		echo "Error: grub-mkrescue not found. You may need to use a Linux VM or Docker to create the ISO."; \
		exit 1; \
	fi

# Create GRUB config
grub.cfg:
	@echo 'set timeout=0' > grub.cfg
	@echo 'set default=0' >> grub.cfg
	@echo '' >> grub.cfg
	@echo 'menuentry "Vibe OS" {' >> grub.cfg
	@echo '    multiboot /boot/kernel.bin' >> grub.cfg
	@echo '    boot' >> grub.cfg
	@echo '}' >> grub.cfg

# Run kernel directly in QEMU (without ISO)
run-kernel: $(KERNEL)
	$(QEMU) -kernel $(KERNEL)

# Run ISO in QEMU
run: $(ISO)
	$(QEMU) -cdrom $(ISO)

# Run in QEMU with debug options
debug: $(KERNEL)
	$(QEMU) -kernel $(KERNEL) -serial stdio -d int -no-reboot

# Clean build files
clean:
	rm -f *.o $(KERNEL) $(ISO)
	rm -rf isodir
	rm -f grub.cfg

.PHONY: all clean run debug iso run-kernel 