#!/bin/sh
make clean
sudo rmmod vfs_module.ko
sudo rm /dev/vfs_module
