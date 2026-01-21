#!/usr/bin/env bash

# project directory $(current file path))
PROJECT_DIR=$(pwd)

# directory of compile output 
BUILD_DIR="$PROJECT_DIR/build"

# build the directory of compile output 
mkdir -p "$BUILD_DIR"

# compile those code.c in the src/
echo "Compiling source files..."
gcc -I"$PROJECT_DIR/include" \
    "$PROJECT_DIR/src/main.c" \
    "$PROJECT_DIR/src/user_hello_linux_test.c" \
    -o "$BUILD_DIR/main.exe"

# check compile successful or not ?
# $? : exit code
if [ $? -eq 0 ]; then
    echo "Build successful! Executable is at $BUILD_DIR/main.exe"
    echo "Running program..."
    "$BUILD_DIR/main.exe"
else
    echo "Build failed!"
    exit 1
fi