#!/bin/sh
make clean
sudo rmmod mmap_example_module.ko
sudo rm /dev/mmap_example_module
