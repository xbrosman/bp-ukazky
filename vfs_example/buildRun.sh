#!/bin/sh
echo "Building..."
gcc vfs_app.c -o vfs_app.app
echo "Build done..."
echo "Run vfs_app.app...\n"
sudo ./vfs_app.app
