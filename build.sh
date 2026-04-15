#!/bin/bash
echo "========================================"
echo "  PowerGame Build Script (Linux/macOS)"
echo "========================================"
echo

if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found. Please install CMake 3.20+"
    exit 1
fi

if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "ERROR: No C++ compiler found."
    exit 1
fi

echo "Configuring project..."
cmake -B build -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed."
    exit 1
fi

echo
echo "Building project..."
cmake --build build --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed."
    exit 1
fi

echo
echo "========================================"
echo "  Build successful!"
echo "  Executable: build/bin/PowerGame"
echo "========================================"
