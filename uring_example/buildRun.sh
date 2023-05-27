#!/bin/sh
echo "Building..."

$appFile = "uring_app.app"

if [-f "$appFile"]; then
    rm simple_app.app
fi
gcc uring_app.c -o uring_app.app
echo "Build done..."
echo "Run uring_app.app...\n"
./uring_app.app
