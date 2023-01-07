/*
    Function networkSetup taken from: https://github.com/mwarning/netlink-examples/blob/master/unicast_example/nl_recv.c
    Author: Moritz Warning

    bp_komunikacia/netlink_example/netlink_app.c
    Author: Filip Brosman
*/
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <linux/netlink.h>

#define DEBUG 0

#define MAX_PAYLOAD 4096 /* maximum payload size */
#define NETLINK_PORT 17

clock_t start, end;
double cpu_time_used;

static size_t SIZE = 4096; // 1MB
char *dataToWrite;
char *dataToRead;

struct sockaddr_nl src_addr;
struct sockaddr_nl dest_addr;
struct nlmsghdr *nlh;
struct msghdr msg;
struct iovec iov;
int sock_fd;
int rc;

void printLog(const char *format, ...);
void printErr(const char *format, ...);

char *prepareDataWrite(size_t);
char *prepareDataRead(size_t);
int prepareData(size_t);

char *changeDataWriteSize(char *, size_t);
char *changeDataReadSize(char *, size_t);
int changeDataSize(size_t);

int writeToDev();
int readFromDev();
double measureFuncDuration(int (*func_ptr)(void));
int doMeasure();

int networkSetup();

int main(int argc, char **argv)
{
    int sizes[5] = {4096,8192,12288,16384, 131072};
    prepareData(SIZE);
    if (networkSetup() == -1)
    {
        printErr("Chyba konfiguracie socketu!");
        return -1;
    }

    for (int i = 0; i < 5; i++)
    {
        SIZE = sizes[i];
        printf("\nMeasurment number: %i, with data size: %liB \n", i+1, SIZE);
        if (changeDataSize(SIZE) != 0)
        {
            printErr("Error in open file: %i\n", sock_fd);
            return -1;
        }
        doMeasure();  
    }


    close(sock_fd);
    free(dataToWrite);
    free(dataToRead);
    return 0;
}

int doMeasure()
{
    double time_taken = 0;

    double sumWrite = 0;
    double avgWrite = 0;

    double sumRead = 0;
    double avgRead = 0;

    int n = 0;
    for (n = 0; n < 100; n++)
    {
        time_taken = measureFuncDuration(writeToDev);
        sumWrite += time_taken;
        //     printLog("Data writen: %s\n", dataToWrite);
        if (time_taken < 0)
        {
            printErr("Error during reading.");
            return -1;
        }

        printLog("Time to write: %fus\n", time_taken * 1000000);

        time_taken = measureFuncDuration(readFromDev);
        sumRead += time_taken;
        //   printLog("Data read: %s\n", dataToRead);
        if (time_taken < 0)
        {
            printErr("Error during reading.");
            return -1;
        }
        printLog("Time to read: %fus\n", time_taken * 1000000);

        if (strcmp(dataToRead, dataToWrite) != 0)
        {
            printErr("Data writen and read are not equal\n");
            return -1;
        }
    }

    avgWrite = sumWrite / n;
    avgRead = sumRead / n;
    printf("avgWrite=%lfus\n", avgWrite * 1000000);
    printf("avgRead=%lfus\n", avgRead * 1000000);
    return 0;
}

double measureFuncDuration(int (*func_ptr)(void))
{
    clock_t t;
    double time_taken;
    int e = 0;

    t = clock();
    e = func_ptr();
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;
    // printf("Send to kernel: %s\n", dataToWrite);
    // printf("Received from kernel: %s\n", NLMSG_DATA(nlh));
    printLog("ret: %li\n", e);
    if (e <= 0)
    {
        printErr("Error in __FUNCTION__ = %s\n", __FUNCTION__);
        return e;
    }
    return time_taken;
}

int writeToDev()
{
    // konfiguracia hlavicky sprav
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(SIZE));
    nlh->nlmsg_len = NLMSG_SPACE(SIZE);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    // naplnenie hlavicky obsahom
    strcpy(NLMSG_DATA(nlh), dataToWrite);

    memset(&iov, 0, sizeof(iov));
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;

    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    // odoslanie do jadra
    rc = sendmsg(sock_fd, &msg, 0);
    if (rc < 0)
    {
        printErr("send: %s\n", strerror(errno));
        close(sock_fd);
        return 0;
    }
    return rc;
}

int readFromDev()
{
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    rc = recvmsg(sock_fd, &msg, 0);
    // kopirovanie dat z jadra kde bol ulozeny funkiou recvmsg()
    strcpy(dataToRead, NLMSG_DATA(nlh));
    if (rc < 0)
    {
        printErr("send: %s\n", strerror(errno));
        close(sock_fd);
        return 0;
    }
    return rc;
}

char *prepareDataWrite(size_t size)
{
    char *dataToWrite = (char *)malloc(size * sizeof(char));
    memset(dataToWrite, 65, size);
    dataToWrite[size + 1] = '\0';
    return dataToWrite;
}

char *prepareDataRead(size_t size)
{
    char *dataToRead = (char *)malloc(size * sizeof(char));
    memset(dataToRead, 0, size);
    return dataToRead;
}

int prepareData(size_t size)
{
    dataToWrite = prepareDataWrite(size);
    dataToRead = prepareDataRead(size);

    if (dataToWrite == NULL || dataToRead == NULL)
        return -1;

    return 0;
}

int networkSetup()
{
    // vytvorenie socketu
    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_PORT);
    if (sock_fd < 0)
    {
        printf("socket: %s\n", strerror(errno));
        return -1;
    }

    // konfikuracia zdrojovej adresy
    memset(&src_addr, 0, sizeof(src_addr));                        // nulovanie pamäte
    src_addr.nl_family = AF_NETLINK;                               // nastavenie AF_NETTLINK socketu
    src_addr.nl_pid = getpid();                                    // pid procesu
    src_addr.nl_groups = 0;                                        // nastavenie skupiny
    bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr)); // zapisanie nastaveni do suboroveho deskriptora

    // konfikuracia cielovej adresy, ciel je jadro
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;
    dest_addr.nl_groups = 0;
    return 0;
}

char *changeDataWriteSize(char* data, size_t size)
{
    char* dataToWrite = (char *)realloc(data, size * sizeof(char));
    memset(dataToWrite, 66, size);
    dataToWrite[size+1] = '\0';
    return dataToWrite;
}

char *changeDataReadSize(char* data, size_t size)
{
    char* dataToRead = (char *)realloc(data, size * sizeof(char));
    memset(dataToRead, 0, size);
    return dataToRead;
}

int changeDataSize(size_t size)
{
    dataToWrite = changeDataWriteSize(dataToWrite, size);
    dataToRead = changeDataReadSize(dataToRead, size);
    if (dataToWrite==NULL || dataToRead==NULL)
        return -1;

    return 0;
}

void printLog(const char *format, ...)
{
    if (DEBUG)
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

void printErr(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
