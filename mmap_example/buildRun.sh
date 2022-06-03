#!/bin/sh
echo "Building..."
gcc mmap_app.c -o mmap_app.app
echo "Build done..."
echo "Run mmap_app.app...\n"
./mmap_app.app
