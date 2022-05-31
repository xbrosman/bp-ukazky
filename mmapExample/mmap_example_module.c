#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/init.h> 
#include <linux/fs.h> 
#include <linux/mm.h> 
#include <asm/uaccess.h>
#include<linux/slab.h>

#define MY_MAJOR 43
#define MY_MAX_MINORS 5
#define MAX_SIZE (PAGE_SIZE*2)
#define NAME "mmap_example_module"

static DEFINE_MUTEX(mchar_mutex);

static char *device_buffer;

static int open(struct inode *pinode, struct file *pfile)
{
    // if(!mutex_trylock(&mchar_mutex)) {
    //     printk(KERN_ALERT "%s: Device is already opened in other device. Can not open.\n", NAME);
	// 	return -1;
    // }
    printk(KERN_INFO "%s: %s\n", NAME,  __FUNCTION__);
    return 0;
}

int close(struct inode *pinode, struct file *pfile)
{    
	// if (mutex_is_locked(&mchar_mutex))
	// 	mutex_unlock(&mchar_mutex);
	printk(KERN_INFO "%s: %s\n",NAME, __FUNCTION__);
    return 0;
}

static ssize_t read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
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

    printk(KERN_INFO "%s: %s, read: %dB\n",NAME, __FUNCTION__, bytes_read);
    return bytes_read;
}

static ssize_t write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
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

    printk(KERN_INFO "%s: %s, written: %dB\n", NAME, __FUNCTION__, bytes_writen);
    return bytes_writen;
}

static int mmap(struct file *filp, struct vm_area_struct *vma)
{
    int ret = 0;
    struct page *page = NULL;
    unsigned long size = (unsigned long)(vma->vm_end - vma->vm_start);

    if (size > MAX_SIZE) {
        ret = -EINVAL;
        goto out;  
    } 
   
    page = virt_to_page((unsigned long)device_buffer + (vma->vm_pgoff << PAGE_SHIFT)); 
    ret = remap_pfn_range(vma, vma->vm_start, page_to_pfn(page), size, vma->vm_page_prot);
 	printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);  
    if (ret != 0) {
        goto out;
    }   
out:
    return ret;
}

// Štruktúra obsahuje vyžšie definované funkcie.
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
	device_buffer = kmalloc(MAX_SIZE, GFP_KERNEL);
    char *text = "Test spojenia s jadrom\0";
    memcpy(device_buffer,text , strlen(text)*sizeof(char));
    
    register_chrdev(MY_MAJOR, NAME, &my_file_operations);
	mutex_init(&mchar_mutex);
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    return 0;
}

void mmap_module_exit(void)
{	
	mutex_destroy(&mchar_mutex);
    unregister_chrdev(MY_MAJOR, NAME);
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
}

module_init(mmap_module_init);
module_exit(mmap_module_exit);
MODULE_LICENSE("GPL");
