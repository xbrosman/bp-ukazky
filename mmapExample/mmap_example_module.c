#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mutex.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mm.h>


#ifndef VM_RESERVED
#define  VM_RESERVED   (VM_DONTEXPAND | VM_DONTDUMP)
#endif

#define MY_MAJOR 43
#define MY_MAX_MINORS 5
#define BUFFER_SIZE 4096
#define NAME "MMAP_Module"

static DEFINE_MUTEX(mmap_mutex);

struct mmap_data
{
    char *data;
    int reference;
};

void mmap_open(struct vm_area_struct *vma)
{
	struct mmap_data *info = (struct mmap_data *)vma->vm_private_data;
	info->reference++;
}

void mmap_close(struct vm_area_struct *vma)
{
	struct mmap_data *info = (struct mmap_data *)vma->vm_private_data;
	info->reference--;
}

// int mmap_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
// {
// 	struct page *page;
// 	struct mmap_data *info;

// 	info = (struct mmap_data *)vma->vm_private_data;
// 	if (!info->data) {
// 		printk("No data\n");
// 		return 0;
// 	}

// 	page = virt_to_page(info->data);

// 	get_page(page);
// 	vmf->page = page;

// 	return 0;
// }

struct vm_operations_struct mmap_vm_ops = {
	.open = mmap_open,
	.close = mmap_close,
	// .fault = mmap_fault,
};

int op_mmap(struct file *filp, struct vm_area_struct *vma)
{
	vma->vm_ops = &mmap_vm_ops;
	vma->vm_flags |= VM_RESERVED;
	vma->vm_private_data = filp->private_data;
	mmap_open(vma);
	return 0;
}

int mmapfop_close(struct inode *inode, struct file *filp)
{
	struct mmap_data *info = filp->private_data;

	free_page((unsigned long)info->data);
	kfree(info);
	filp->private_data = NULL;

	mutex_unlock(&mmap_mutex);

	return 0;
}

int mmapfop_open(struct inode *inode, struct file *filp)
{
	struct mmap_data *info = NULL;

	if (!mutex_trylock(&mmap_mutex)) {
		printk(KERN_WARNING
		       "Another process is accessing the device\n");
		return -EBUSY;
	}

	info = kmalloc(sizeof(struct mmap_data), GFP_KERNEL);
	info->data = (char *)get_zeroed_page(GFP_KERNEL);
	memcpy(info->data, "Hello from kernel this is file: ", 32);
	memcpy(info->data + 32, filp->f_path.dentry->d_name.name,
	       strlen(filp->f_path.dentry->d_name.name));
	/* assign this info struct to the file */
	filp->private_data = info;
	return 0;
}

static const struct file_operations mmap_fops = {
	.owner = THIS_MODULE,
	.open = mmapfop_open,
	.release = mmapfop_close,
	.mmap = op_mmap,
};

int mmap_module_init(void)
{
	mutex_init(&mmap_mutex);
  	printk(KERN_INFO "%s: %s\n",NAME, __FUNCTION__);
    register_chrdev(MY_MAJOR,NAME, &mmap_fops);
    return 0;
}

void mmap_module_exit(void)
{
    printk(KERN_INFO "%s: %s\n",NAME, __FUNCTION__);
    unregister_chrdev(MY_MAJOR, NAME);
}

module_init(mmap_module_init);
module_exit(mmap_module_exit);
MODULE_LICENSE("GPL");
