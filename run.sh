#!/bin/bash
# Script to build and run Vibe OS on macOS

# Check for required tools
check_tool() {
    if ! command -v $1 &> /dev/null; then
        echo "Error: $1 is not installed. Please install it first."
        echo "You can use: brew install $2"
        exit 1
    fi
}

check_tool "x86_64-elf-gcc" "x86_64-elf-gcc"
check_tool "x86_64-elf-as" "x86_64-elf-binutils"
check_tool "x86_64-elf-ld" "x86_64-elf-binutils"
check_tool "qemu-system-i386" "qemu"

# Clean previous build
echo "Cleaning previous build..."
make clean

# Build the kernel
echo "Building kernel..."
make

# Run the kernel directly in QEMU
echo "Running Vibe OS in QEMU..."
make run-kernel

echo "Done!" 