#!/bin/sh
make clean
sudo rmmod uring_module.ko
sudo rm /dev/simple_chardev
