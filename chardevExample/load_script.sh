#!/bin/sh

sudo insmod simple_module.ko
sudo mknod -m 777 /dev/simple_chardev c 42 0
