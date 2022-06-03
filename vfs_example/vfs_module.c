/*
    SPDX-License-Identifier: GPL-2.0
    bp_komunikacia/vfs_example/vfs_module.c
    Module taken from: https://pradheepshrinivasan.github.io/2015/07/02/Creating-an-simple-sysfs/
    Author: 

    Modified by Filip Brosman

*/
#include <linux/init.h> 
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/printk.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include <linux/string.h>

#define MAX_SIZE (PAGE_SIZE*2)
#define NAME "vfs_module"

static struct kobject *example_kobject;
static int value;

static ssize_t value_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", value);
}

static ssize_t value_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    sscanf(buf, "%du", &value);
    return count;
}



int vfs_module_init(void)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    int e;
    static struct kobj_attribute value_attribute = __ATTR(value, 0660, value_show, value_store);
    example_kobject = kobject_create_and_add("my_value", kernel_kobj);
    if(!example_kobject)
        return -ENOMEM;

    e = sysfs_create_file(example_kobject, &value_attribute.attr);
    if (e){
        pr_debug("failed to create the value file");
    }
    return e;
}

void vfs_module_exit(void)
{	
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    kobject_put(example_kobject);
}

module_init(vfs_module_init);
module_exit(vfs_module_exit);
MODULE_LICENSE("GPL");
