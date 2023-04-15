#!/bin/sh
echo "Building..."

gcc signals_example.c -o signals_example.app -g
echo "Build done..."
echo "Run signals_example.app...\n"
./signals_example.app
