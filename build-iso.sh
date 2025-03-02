#!/bin/bash
set -e

echo "Building Vibe OS ISO..."

# Compile the kernel
echo "Compiling kernel..."
gcc -m32 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -nostdlib -nostdinc -c kernel.c -o kernel.o
as --32 boot.s -o boot.o
ld -m elf_i386 -n -T linker.ld -nostdlib boot.o kernel.o -o kernel.bin

# Create GRUB configuration
echo "Creating GRUB configuration..."
mkdir -p isodir/boot/grub
cp kernel.bin isodir/boot/
cat > isodir/boot/grub/grub.cfg << EOF
set timeout=0
set default=0

menuentry "Vibe OS" {
    multiboot /boot/kernel.bin
    boot
}
EOF

# Create the ISO
echo "Creating ISO image..."
grub-mkrescue -o vibe-os.iso isodir

echo "ISO created successfully: vibe-os.iso"
ls -lh vibe-os.iso 