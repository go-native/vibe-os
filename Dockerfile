FROM --platform=linux/amd64 ubuntu:22.04

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    binutils \
    gcc \
    gcc-multilib \
    libc6-dev-i386 \
    xorriso \
    grub-pc-bin \
    grub-common \
    nasm \
    && rm -rf /var/lib/apt/lists/*

# Set up working directory
WORKDIR /vibe-os

# The source files will be mounted at runtime
ENTRYPOINT ["bash", "/vibe-os/build-iso.sh"] 