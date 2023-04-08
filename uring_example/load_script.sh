#!/bin/sh
make
sudo insmod uring_module.ko
#sudo mknod -m 777 /dev/uring_chardev c 42 0
