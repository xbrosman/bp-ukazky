#!/bin/sh
make clean
sudo rmmod netlink_module.ko
make
sudo insmod netlink_module.ko
