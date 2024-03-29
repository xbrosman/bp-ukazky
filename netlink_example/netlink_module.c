/*
    SPDX-License-Identifier: GPL-2.0
    bp_komunikacia/netlink_example/netlink_module.c
    Module taken from: https://github.com/mwarning/netlink-examples/tree/master/unicast_example
    Author: Moritz Warning

    Modified by Filip Brosman
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NAME "netlink_module"
#define NETLINK_PORT 17

struct sock *nl_sock = NULL;

// listener na primanie sprav, echo funkcia
static void nl_rec_msg(struct sk_buff *skb)
{
    struct sk_buff *skb_out;
    struct nlmsghdr *nlh;
    int msg_size;
    char *msg;
    int pid;
    int res;

    nlh = (struct nlmsghdr *)skb->data;
    pid = nlh->nlmsg_pid;
    msg = (char *)nlmsg_data(nlh); // hlavicka spravy
    msg_size = strlen(msg);

    // printk(KERN_INFO "%s: Received from pid %i\n", NAME, pid);
    // printk(KERN_INFO "%s: Received from pid %i: %s\n", NAME, pid, msg);
    skb_out = nlmsg_new(msg_size, 0);
    if (!skb_out)
    {
        printk(KERN_ERR "%s: Failed to allocate new skb\n", NAME);
        return;
    }

    // ulozi prijatu spravu do obasahu na odoslanie
    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0;
    strncpy(nlmsg_data(nlh), msg, msg_size);

    // printk(KERN_INFO "%s: Send %s\n", NAME, msg);

    // odoslanie spravy
    res = nlmsg_unicast(nl_sock, skb_out, pid);
    if (res < 0)
        printk(KERN_INFO "%s: Error while sending skb to user\n", NAME);
}

int netlink_module_init(void)
{
    struct netlink_kernel_cfg nl_config = {
        .input = nl_rec_msg,
    };
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
