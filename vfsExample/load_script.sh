#!/bin/sh
make
sudo insmod vfs_module.ko
sudo mknod -m 777 /dev/vfs_module c 44 0
