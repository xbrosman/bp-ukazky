#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>  
#include <linux/uaccess.h> 
#include <linux/fs.h>

#define MY_MAJOR 44
#define MY_MAX_MINORS 5
#define BUFFER_SIZE 4096
#define NAME "vfs_exmaple"


static char device_buffer[BUFFER_SIZE];

int open(struct inode *pinode, struct file *pfile)
{
    printk(KERN_INFO "%s: %s\n", NAME,  __FUNCTION__);
    return 0;
}

ssize_t read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
    printk(KERN_INFO "%s: %s\n",NAME, __FUNCTION__);
    return 0;
}

ssize_t write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
    printk(KERN_INFO "%s: %s\n",NAME, __FUNCTION__);
    return 0;
}

int close(struct inode *pinode, struct file *pfile)
{    
    printk(KERN_INFO "%s: %s\n",NAME, __FUNCTION__);
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

int vfs_module_init(void)
{
    register_chrdev(MY_MAJOR, NAME, &my_file_operations);
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    return 0;
}

void vfs_module_exit(void)
{
    unregister_chrdev(MY_MAJOR, NAME);
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
}

module_init(vfs_module_init);
module_exit(vfs_module_exit);
MODULE_LICENSE("GPL");
