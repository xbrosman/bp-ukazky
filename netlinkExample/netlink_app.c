#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define MAX_PAYLOAD 4096 /* maximum payload size */
#define NETLINK_PORT 17

#define SIZE 4096
char *dataToWrite;
char *dataToRead;

struct sockaddr_nl src_addr;
struct sockaddr_nl dest_addr;
struct nlmsghdr *nlh;
struct msghdr msg;
struct iovec iov;
int sock_fd;
int rc;

void prepareData()
{
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */
    src_addr.nl_groups = 0;     /* not in mcast groups */
    bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;    /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    dataToWrite = (char *)malloc(SIZE * sizeof(char));
    int i;
    for (i = 0; i < SIZE; i++)
    {
        dataToWrite[i] = 'A' + (char)(i % 26);
    }
    dataToWrite[SIZE] = '\0';
    dataToRead = (char *)malloc(SIZE * sizeof(char));
    memset(dataToRead, 0, sizeof(dataToRead));

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));

    /* Fill the netlink message header */
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid(); /* self pid */
    nlh->nlmsg_flags = 0;

    /* Fill in the netlink message payload */
    strcpy(NLMSG_DATA(nlh), dataToWrite);

    memset(&iov, 0, sizeof(iov));
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;

    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // printf("\n %s %s\n", dataToRead, dataToWrite);
}

void writeToDev()
{
    rc = sendmsg(sock_fd, &msg, 0);
    if (rc < 0)
    {
        printf("sendmsg(): %s\n", strerror(errno));
        close(sock_fd);
        return;
    }
    //printf("Send to kernel: %s\n", dataToWrite);
}

void readFromDev()
{
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

    rc = recvmsg(sock_fd, &msg, 0);
    if (rc < 0)
    {
        printf("sendmsg(): %s\n", strerror(errno));
        close(sock_fd);
        return;
    }
   // printf("Received from kernel: %s\n", NLMSG_DATA(nlh));
}

int main(int argc, char **argv)
{
    clock_t t;
    double time_taken;

    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_PORT);
    if (sock_fd < 0)
    {
        printf("socket: %s\n", strerror(errno));
        return 1;
    }
    prepareData();

    t = clock();
    writeToDev();
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;
    printf("Cas na vykonanie zapisu: %fus\n", time_taken * 1000000);

    t = clock();
    readFromDev();
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;
    printf("Cas na vykonanie citania: %fus\n", time_taken * 1000000);

    /* Close Netlink Socket */
    close(sock_fd);
    free(dataToWrite);
    free(dataToRead);

    return 0;
}