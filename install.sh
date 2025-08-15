#!/bin/bash

set -e  # Exit on any error

# Ensure the script is run as root
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root"
    exit 1
fi

# Define installation directories
LIB_NAME="libLumos.so"
LIB_DIR="/usr/local/lib"
INCLUDE_DIR="/usr/local/include/Lumos"
PKGCONFIG_DIR="/usr/lib/pkgconfig"
PKGCONFIG_FILE="Lumos.pc"

echo "Compiling the Lumos library..."

# Compile the library
make clean 2>/dev/null || true  # Clean previous builds if Makefile supports it
make -j$(nproc)

echo "Installing the library..."

# Create directories if they don't exist
sudo mkdir -p "$LIB_DIR"
sudo mkdir -p "$INCLUDE_DIR"
sudo mkdir -p "$PKGCONFIG_DIR"

# Copy the shared library
cp -r "$LIB_NAME" "$LIB_DIR"

# Copy header files
cp -r lib/* "$INCLUDE_DIR"
# cp -r lib/*/*.h "$INCLUDE_DIR"

# Create Lumos.pc file
echo "Creating $PKGCONFIG_FILE..."
cat <<EOF | sudo tee "$PKGCONFIG_DIR/$PKGCONFIG_FILE" > /dev/null
prefix=/usr/local
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include/Lumos

Name: Lumos
Description: A custom library for various utilities
Version: 2.0.0
Libs: -L\${libdir} -lLumos
Cflags: -I\${includedir}
EOF

# Update library cache
echo "Updating linker cache..."
ldconfig

echo "Lumos library installed/updated successfully."
