#!/bin/sh
make clean
sudo rmmod uring_module.ko
sudo rm /dev/uring_chardev
make
sudo insmod uring_module.ko
#sudo mknod -m 777 /dev/uring_chardev c 42 0
