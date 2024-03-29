/*
    SPDX-License-Identifier: GPL-2.0
    bp_komunikacia/mmap_example/mmap_example_module.c

    Functions read, write taken from github: https://gist.github.com/ksvbka/0cd1a31143c1003ce6c7
    Author: Anil Kumar Pugalia <email@sarika-pugs.com>

    Function taken from github: https://gist.github.com/laoar/4a7110dcd65dbf2aefb3231146458b39
    Author: Yafang Shao

    Modified by Filip Brosman

*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#define MY_MAJOR 43
#define MY_MAX_MINORS 5
#define MAX_SIZE 1024 * 4096 // 4MB Dát
#define NAME "mmap_example_module"

static DEFINE_MUTEX(mmap_mutex);

static char *device_buffer;

static int open(struct inode *pinode, struct file *pfile)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    if (!mutex_trylock(&mmap_mutex))
    {
        printk(KERN_ALERT "%s: Device is already opened. Can not be opened.\n", NAME);
        return -1;
    }
    return 0;
}

int close(struct inode *pinode, struct file *pfile)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    if (mutex_is_locked(&mmap_mutex))
        mutex_unlock(&mmap_mutex);
    return 0;
}

static ssize_t read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    int b_max;
    int bytes_to_read;
    int bytes_read;
    b_max = MAX_SIZE - *offset;
    if (b_max > length)
        bytes_to_read = length;
    else
        bytes_to_read = b_max;
    if (bytes_to_read == 0)
        printk(KERN_INFO "%s: Reached the end of the device\n", NAME);

    bytes_read = bytes_to_read - copy_to_user(buffer, device_buffer + *offset, bytes_to_read);
    *offset += bytes_read;

    return bytes_read;
}

static ssize_t write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    int b_max;
    int bytes_to_write;
    int bytes_writen;
    b_max = MAX_SIZE - *offset;
    if (b_max > length)
        bytes_to_write = length;
    else
        bytes_to_write = b_max;

    bytes_writen = bytes_to_write - copy_from_user(device_buffer + *offset, buffer, bytes_to_write);
    *offset += bytes_writen;

    return bytes_writen;
}

static int mmap(struct file *filp, struct vm_area_struct *vma)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    int ret = 0;
    struct page *page = NULL;
    unsigned long size = (unsigned long)(vma->vm_end - vma->vm_start);

    if (size > MAX_SIZE)
    {
        ret = -EINVAL;
        return ret;
    }

    page = virt_to_page((unsigned long)device_buffer + (vma->vm_pgoff << PAGE_SHIFT));
    ret = remap_pfn_range(vma, vma->vm_start, page_to_pfn(page), size, vma->vm_page_prot);
    if (ret != 0)
    {
        return ret;
    }
    return ret;
}

struct file_operations my_file_operations = {
    .owner = THIS_MODULE,
    .open = open,
    .read = read,
    .write = write,
    .mmap = mmap,
    .release = close,
};

int mmap_module_init(void)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    device_buffer = (char *)kmalloc(MAX_SIZE, GFP_KERNEL);
    // char *text = "Message from kernel\0";
    // memcpy(device_buffer,text , strlen(text)*sizeof(char));

    register_chrdev(MY_MAJOR, NAME, &my_file_operations);
    mutex_init(&mmap_mutex);
    return 0;
}

void mmap_module_exit(void)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    mutex_destroy(&mmap_mutex);
    unregister_chrdev(MY_MAJOR, NAME);
}

module_init(mmap_module_init);
module_exit(mmap_module_exit);
MODULE_LICENSE("GPL");
