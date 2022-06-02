#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>  
#include <linux/uaccess.h> 
#include <linux/fs.h>

#define MY_MAJOR 42
#define MY_MAX_MINORS 5
#define BUFFER_SIZE 4096
#define NAME "Simple_chardev"


static char device_buffer[BUFFER_SIZE];
struct semaphore sem;

int open(struct inode *pinode, struct file *pfile)
{
    printk(KERN_INFO "%s: %s\n", NAME,  __FUNCTION__);
    if (down_interruptible(&sem) != 0)
    {
        printk(KERN_ALERT "%s: Device is already opened in other device. Can not open.\n", NAME);
        return -1;
    }
    return 0;
}

ssize_t read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
    printk(KERN_INFO "%s: %s, read: %dB\n",NAME, __FUNCTION__, bytes_read);
    int b_max;
    int bytes_to_read;
    int bytes_read;
    b_max = BUFFER_SIZE - *offset;
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

ssize_t write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
    printk(KERN_INFO "%s: %s, written: %dB\n", NAME, __FUNCTION__, bytes_writen);
    int b_max;
    int bytes_to_write;
    int bytes_writen;
    b_max = BUFFER_SIZE - *offset;
    if (b_max > length)
        bytes_to_write = length;
    else
        bytes_to_write = b_max;

    bytes_writen = bytes_to_write - copy_from_user(device_buffer + *offset, buffer, bytes_to_write);
    *offset += bytes_writen;

    return bytes_writen;
}

int close(struct inode *pinode, struct file *pfile)
{    
    printk(KERN_INFO "%s: %s\n",NAME, __FUNCTION__);
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
    return 0;
}

void simple_module_exit(void)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    unregister_chrdev(MY_MAJOR, NAME);
}

module_init(simple_module_init);
module_exit(simple_module_exit);
MODULE_LICENSE("GPL");
