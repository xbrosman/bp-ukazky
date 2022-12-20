/*
    SPDX-License-Identifier: GPL-2.0
    bp_komunikacia/chardev_example/simple_module.c

    Functions read, write taken from github: https://gist.github.com/ksvbka/0cd1a31143c1003ce6c7
    Author: Anil Kumar Pugalia <email@sarika-pugs.com>
    Modified by Filip Brosman

*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/slab.h>

#define MY_MAJOR 42
#define MY_MAX_MINORS 5
#define BUFFER_SIZE 1024*4096    // 4MB Dát
#define NAME "Simple_chardev"


static char *device_buffer = NULL;
struct semaphore sem;

int open(struct inode *pinode, struct file *pfile)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    if (down_interruptible(&sem) != 0)
    {
        printk(KERN_ALERT "%s: Device is already opened in other device. Can not open.\n", NAME);
        return -1;
    }
    return 0;
}

ssize_t read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
    int bytes_read = 0;
    int bytes_to_read = 0;
    if (length >= BUFFER_SIZE)
    {
        bytes_to_read = BUFFER_SIZE;   
    }         
    else
    {
        bytes_to_read = length;
    }    

    bytes_read = copy_to_user(buffer+*offset, device_buffer, bytes_to_read);
    if (bytes_read != 0)
    {
        printk(KERN_INFO "Error read: %s", __FUNCTION__);
        return -EFAULT; // vracia bad address
    }
    //printk(KERN_INFO "%s: %s, %iB, %iB\n",NAME, __FUNCTION__, bytes_to_read, bytes_to_read-bytes_read);
    return bytes_to_read - bytes_read;
}

ssize_t write(struct file *pfile, const char *buffer, size_t length, loff_t *offset)
{
    int bytes_writen = 0;  
    int bytes_to_write = 0;   
    if (length >= BUFFER_SIZE)
    {
        bytes_to_write = BUFFER_SIZE;   
    }         
    else
    {
        bytes_to_write = length;
    }   
    bytes_writen = copy_from_user(device_buffer + *offset, buffer, bytes_to_write);
    //printk(KERN_INFO "%s: %s, %iB, %iB\n", NAME, __FUNCTION__, bytes_to_write, bytes_to_write-bytes_writen);    
    return bytes_to_write - bytes_writen;
}

int close(struct inode *pinode, struct file *pfile)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    up(&sem);
    return 0;
}

// Štruktúra obsahuje vyžšie definované funkcie.
struct file_operations my_file_operations = {
    .owner = THIS_MODULE,
    .open = open,
    .read = read,
    .write = write,
    .release = close,
};

int simple_module_init(void)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    sema_init(&sem, 1);
    register_chrdev(MY_MAJOR, NAME, &my_file_operations);
    device_buffer = (char *)kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (device_buffer != NULL)
        printk("malloc address: 0x%p\n", device_buffer);
    else
        printk("wrong malloc address: 0x%p\n", device_buffer);

    return 0;
}

void simple_module_exit(void)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    unregister_chrdev(MY_MAJOR, NAME);
    if (device_buffer)
        kfree(device_buffer);
}

module_init(simple_module_init);
module_exit(simple_module_exit);
MODULE_LICENSE("GPL");
