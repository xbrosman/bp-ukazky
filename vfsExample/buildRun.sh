#!/bin/sh
echo "Building..."
gcc vfs_app.c -o vfs_app.app
echo "Build done..."
echo "Run simple_app.app...\n"
./vfs_app.app
