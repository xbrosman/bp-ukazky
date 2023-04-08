/*
    SPDX-License-Identifier: GPL-2.0
    bp_komunikacia/uring_example/uring_module.c

    Author: Filip Brosman
    Modified by Filip Brosman

*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/slab.h>

#define MY_MAJOR 44
#define MY_MAX_MINORS 5
#define BUFFER_SIZE 1024*4096    // 4MB Dát
#define NAME "Simple_chardev"


static char *device_buffer = NULL;

int uring_module_init(void)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    device_buffer = (char *)kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (device_buffer != NULL)
        printk("malloc address: 0x%p\n", device_buffer);
    else
        printk("wrong malloc address: 0x%p\n", device_buffer);

    return 0;
}

void uring_module_exit(void)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    if (device_buffer)
        kfree(device_buffer);
}

module_init(uring_module_init);
module_exit(uring_module_exit);
MODULE_LICENSE("GPL");
