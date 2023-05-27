#!/bin/sh
echo "Building..."

gcc uring_app.c -o uring_app.app -g -luring
echo "Build done..."
echo "Run uring_app.app...\n"
./uring_app.app
