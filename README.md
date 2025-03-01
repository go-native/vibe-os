# Vibe OS

A simple 32-bit operating system that boots from an ISO image and displays a welcome message.

## Prerequisites

To build and run Vibe OS, you'll need:

- GCC cross-compiler for x86_64 (with 32-bit support)
- GNU Make
- QEMU (for testing)
- Optional: GRUB2 (with grub-mkrescue) and xorriso (for creating bootable ISO)

### macOS Setup

On macOS, you can install the required tools using Homebrew:

```bash
# Install QEMU
brew install qemu

# Install cross-compiler tools
brew install x86_64-elf-gcc
brew install x86_64-elf-binutils
```

## Building

To build the kernel:

```bash
make
```

This will compile the assembly and C source files and link them into a kernel binary.

## Running

To run Vibe OS directly in QEMU (without creating an ISO):

```bash
make run-kernel
```

Alternatively, you can use the provided shell script which checks for dependencies, builds the kernel, and runs it in QEMU:

```bash
./run.sh
```

## Creating a Bootable ISO

Creating a bootable ISO requires GRUB2 with grub-mkrescue, which is not easily available on macOS. If you need to create an ISO, you might need to use a Linux VM or Docker.

If you have grub-mkrescue available, you can create an ISO with:

```bash
make iso
```

And then run it with:

```bash
make run
```

## Project Structure

- `boot.s` - Assembly code for the multiboot header and kernel entry
- `kernel.c` - Main kernel code that displays the welcome message
- `linker.ld` - Linker script for memory layout
- `Makefile` - Build system configuration
- `run.sh` - Helper script to build and run the OS

## Extending the OS

This is a minimal OS that only displays a welcome message. To extend it, you might want to add:

1. Keyboard input handling
2. Memory management
3. File system support
4. Process management
5. More device drivers

## License

This project is open source and available under the MIT License. 