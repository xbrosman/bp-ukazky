#!/bin/sh
echo "Building..."
gcc simple_app.c -o simple_app.app
echo "Build done..."
echo "Run simple_app.app...\n"
./simple_app.app
