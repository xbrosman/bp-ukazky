#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/printk.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include <linux/string.h>

#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NAME "netlink_module"
#define NETLINK_PORT 17

struct sock *nl_sock = NULL;

static void nl_rec_msg(struct sk_buff *skb)
{
    int msg_size;
    char *msg;
    int pid;
    int res;
    struct sk_buff *skb_out;
    struct nlmsghdr *nlh;

    nlh = (struct nlmsghdr *)skb->data;
    pid = nlh->nlmsg_pid;
    msg = (char *)nlmsg_data(nlh);
    msg_size = strlen(msg);

    printk(KERN_INFO "%s: Received from pid %i: %s\n", NAME, pid, msg);

    skb_out = nlmsg_new(msg_size, 0);
    if (!skb_out)
    {
        printk(KERN_ERR "%s: Failed to allocate new skb\n", NAME);
        return;
    }

    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0;
    strncpy(nlmsg_data(nlh), msg, msg_size);

    printk(KERN_INFO "%s: Send %s\n", NAME, msg);

    res = nlmsg_unicast(nl_sock, skb_out, pid);
    if (res < 0)
        printk(KERN_INFO "%s: Error while sending skb to user\n", NAME);
}

struct netlink_kernel_cfg nl_config = {
    .input = nl_rec_msg,
};

int netlink_module_init(void)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);

    nl_sock = netlink_kernel_create(&init_net, NETLINK_PORT, &nl_config);
    if (!nl_sock)
    {
        printk(KERN_ALERT "%s: Error creating socket.\n", NAME);
        return -1;
    }
    return 0;
}

void netlink_module_exit(void)
{
    printk(KERN_INFO "%s: %s\n", NAME, __FUNCTION__);
    netlink_kernel_release(nl_sock);
}

module_init(netlink_module_init);
module_exit(netlink_module_exit);
MODULE_LICENSE("GPL");
