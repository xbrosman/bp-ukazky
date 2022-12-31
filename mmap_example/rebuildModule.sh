#!/bin/sh
make clean
sudo rmmod mmap_example_module.ko
sudo rm /dev/mmap_example_module
make
sudo insmod mmap_example_module.ko
sudo mknod -m 777 /dev/mmap_example_module c 43 0
