#!/bin/sh
echo "Building..."
gcc netlink_app.c -o netlink_app.app -Wall
echo "Build done..."
echo "Run netlink_app.app...\n"
./netlink_app.app
