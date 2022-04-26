#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

// lubovolne číslo 
#define MY_MAJOR       42
#define MY_MAX_MINORS  5

MODULE_LICENSE("GPL");

int open (struct inode *pinode, struct file *pfile){
    printk(KERN_ALERT "Chardev module %s\n", __FUNCTION__);
    return 0;
}

ssize_t read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset ){
    printk(KERN_ALERT "Chardev module %s\n", __FUNCTION__);
    return 0;
}

ssize_t write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset ){
    printk(KERN_ALERT "Chardev module %s\n", __FUNCTION__);
    return length;
}

int close (struct inode *pinode, struct file *pfile){
    printk(KERN_ALERT "Chardev module %s\n", __FUNCTION__);
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
    printk(KERN_ALERT "Simple chardev module %s\n", __FUNCTION__);   
    register_chrdev(MY_MAJOR,"Example Chardev", &my_file_operations);
    return 0;
}

void simple_module_exit(void)
{
    printk(KERN_ALERT "Simple chardev module %s\n", __FUNCTION__);
    unregister_chrdev(MY_MAJOR, "Example Chardev");
}

module_init(simple_module_init);
module_exit(simple_module_exit);
