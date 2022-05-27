#!/bin/sh
make clean
sudo rmmod simple_module.ko
sudo rm /dev/simple_chardev
