#!/bin/sh
echo "Building..."

$appFile = "simple_app.app"

if [-f "$appFile"]; then
    rm simple_app.app
fi
gcc simple_app.c -o simple_app.app
echo "Build done..."
echo "Run simple_app.app...\n"
./simple_app.app
