/*

*/
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

int networkSetup()
{
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_PORT);
    if (sock_fd < 0)
    {
        printf("socket: %s\n", strerror(errno));
        return 1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    src_addr.nl_groups = 0;
    bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;
    dest_addr.nl_groups = 0; 

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));

    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    strcpy(NLMSG_DATA(nlh), dataToWrite);

    memset(&iov, 0, sizeof(iov));
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;

    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    return 0;
}

void prepareData()
{
    dataToWrite = (char *)malloc(SIZE * sizeof(char));
    int i;
    for (i = 0; i < SIZE; i++)
    {
        dataToWrite[i] = 'A' + (char)(i % 26);
    }
    dataToWrite[SIZE] = '\0';
    dataToRead = (char *)malloc(SIZE * sizeof(char));
    memset(dataToRead, 0, sizeof(dataToRead));  
    // printf("\n %s %s\n", dataToRead, dataToWrite);
}

int writeToDev()
{
    rc = sendmsg(sock_fd, &msg, 0);
    if (rc < 0)
    {
        printf("send: %s\n", strerror(errno));
        close(sock_fd);
        return 1;
    }
    //printf("Send to kernel: %s\n", dataToWrite);
    return 0;
}

int readFromDev()
{
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

    rc = recvmsg(sock_fd, &msg, 0);
    if (rc < 0)
    {
        printf("send: %s\n", strerror(errno));
        close(sock_fd);
        return 1;
    }
   // printf("Received from kernel: %s\n", NLMSG_DATA(nlh));
   return 0;
}

int main(int argc, char **argv)
{
    clock_t t;
    double time_taken;
    int e = 0;
    prepareData();
    if (e = networkSetup())
        goto freeall;

    t = clock();
    e = writeToDev();
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;
    if (e)
    {
        printf("Error during writing.");
        goto freeall;
    }
    else
    {
        printf("Time to write: %fus\n", time_taken * 1000000);       
    }

    t = clock();
    e = readFromDev();
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;
    if (e)
    {   
       printf("Error during reading.");
       goto freeall;
    }
    else
    {
        printf("Time to read: %fus\n", time_taken * 1000000);
    }
    close(sock_fd);
    free(dataToWrite);
    free(dataToRead);
    return 0;
freeall:
    close(sock_fd);
    free(dataToWrite);
    free(dataToRead);
    return e;
}
