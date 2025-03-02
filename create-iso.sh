#!/bin/bash
set -e

echo "Building Docker image for ISO creation..."
docker build --platform linux/amd64 -t vibe-os-builder .

echo "Running Docker container to create ISO..."
docker run --platform linux/amd64 --rm -v "$(pwd):/vibe-os" vibe-os-builder

echo "ISO creation complete!"
echo "You can now use vibe-os.iso with Proxmox VM." 