#!/bin/sh
make clean
sudo rmmod simple_module.ko
sudo rm /dev/simple_chardev
make
sudo insmod simple_module.ko
sudo mknod -m 777 /dev/simple_chardev c 42 0
