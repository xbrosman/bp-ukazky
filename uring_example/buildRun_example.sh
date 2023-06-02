#!/bin/sh
echo "Building..."

gcc uring_example.c -o uring_example.app -g -luring
echo "Build done..."
echo "Run uring_example.app...\n"
echo $1 
echo $2
./uring_example.app $1 $2
