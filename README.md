# Vibe OS

A simple 32-bit operating system that boots from an ISO image and displays a welcome message.

## Prerequisites

To build and run Vibe OS, you'll need:

- GCC cross-compiler for x86_64 (with 32-bit support)
- GNU Make
- QEMU (for testing)
- Docker (for creating bootable ISO)

### macOS Setup

On macOS, you can install the required tools using Homebrew:

```bash
# Install QEMU
brew install qemu

# Install cross-compiler tools
brew install x86_64-elf-gcc
brew install x86_64-elf-binutils

# Install Docker
brew install --cask docker
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

## Creating a Bootable ISO for Proxmox VM

To create a bootable ISO that can be used with Proxmox VM, we use Docker to provide the necessary tools (GRUB, xorriso) that aren't easily available on macOS:

1. Make sure Docker is installed and running
2. Run the provided script:

```bash
./create-iso.sh
```

This will:
- Build a Docker image with the necessary tools
- Mount your current directory into the container
- Compile the kernel and create a bootable ISO
- Save the ISO as `vibe-os.iso` in your current directory

You can then upload this ISO to your Proxmox server and create a VM that boots from it.

## Using the ISO with Proxmox

1. Upload the ISO to your Proxmox server's ISO storage
2. Create a new VM with the following settings:
   - OS: Other
   - CD/DVD: Select the vibe-os.iso
   - Memory: 512MB (minimum)
   - Disk: Not required for this basic OS
3. Start the VM and you should see the Vibe OS welcome message

## Project Structure

- `boot.s` - Assembly code for the multiboot header and kernel entry
- `kernel.c` - Main kernel code that displays the welcome message
- `linker.ld` - Linker script for memory layout
- `Makefile` - Build system configuration
- `run.sh` - Helper script to build and run the OS in QEMU
- `Dockerfile` - Docker configuration for ISO creation
- `build-iso.sh` - Script that runs inside Docker to create the ISO
- `create-iso.sh` - Script to build and run the Docker container

## How ISO Creation Works

The ISO creation process works as follows:

1. The `create-iso.sh` script builds a Docker image with all necessary tools
2. It then runs a container from this image, mounting your project directory
3. Inside the container, `build-iso.sh` runs to:
   - Compile the kernel
   - Create a GRUB configuration file
   - Package everything into a bootable ISO using grub-mkrescue
4. The resulting ISO is saved to your project directory

This approach allows us to use Linux-specific tools like grub-mkrescue on macOS.

## Extending the OS

This is a minimal OS that only displays a welcome message. To extend it, you might want to add:

1. Keyboard input handling
2. Memory management
3. File system support
4. Process management
5. More device drivers

## License

This project is open source and available under the MIT License. 