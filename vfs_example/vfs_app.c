/*
    bp_komunikacia/vfs_example/vfs_app.c
    Author: Filip Brosman
*/
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

clock_t start, end;
double cpu_time_used;

#define DEVICE "/sys/kernel/my_value/value"
#define SIZE 4096
int fd = 0;
int offset = 0;
char dataToWrite;
char *dataToRead;

void prepareData()
{
    fd = open(DEVICE, O_RDWR);
    printf("%i\n", fd);

    dataToWrite = 1;
    dataToRead = (char *)malloc(SIZE * sizeof(char));
    memset(dataToRead, 0, sizeof(dataToRead));
}

int writeToDev()
{
    ssize_t res;
    res = write(fd, &dataToWrite, sizeof(dataToWrite), &offset);
    if (res == -1)
    {
        printf("Error during write...\n");
        return -1;
    }
    // printf("Zapisanych: %liB %i...\n", res, dataToWrite);
    return 0;
}

int readFromDev()
{
    ssize_t res;
    res = read(fd, dataToRead, SIZE, &offset);
    if (res == -1)
    {
        printf("Error during read...\n");
        return -1;
    }
    // printf("Precitaných: %liB %i... \n", res, dataToRead);
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
    if (e == -1)
        return -1;
    return time_taken;
}

int openDev(char *device)
{
    if (access(device, F_OK) == -1)
    {
        printf("Module %s not loaded... Close\n", device);
        return -2;
    }
    printf("Module %s loaded... \n", DEVICE);
    return 0;
}

int main(int argc, char const *argv[])
{
    int e;
    double time;

    if ((e = openDev(DEVICE)) != 0)
    {
        return e;
    }
    prepareData();

    time = measureFuncDuration(&writeToDev);

    if (time == -1)
    {
        printf("Error during write.\n");
        goto freeall;
    }
    else
    {
        printf("Time to write: %fus\n", time * 1000000);
    }

    time = measureFuncDuration(&readFromDev);
    if (time == -1)
    {
        printf("Error during reading.\n");
        goto freeall;
    }
    else
    {
        printf("Time to read: %fus\n", time * 1000000);
    }

    close(fd);
    free(dataToRead);
    return 0;
freeall:
    close(fd);
    free(dataToRead);
    return e;
}
