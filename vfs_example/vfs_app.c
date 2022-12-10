/*
    bp_komunikacia/vfs_example/vfs_app.c
    Author: Filip Brosman
*/
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

clock_t start, end;
double cpu_time_used;

#define DEVICE "/sys/kernel/my_value/value"
#define SIZE 4096
int fd = 0;
int offset = 0;
char *dataToWrite;
char *dataToRead;

int openDev(char *device);
void prepareData(int size);
int writeToDev();
int readFromDev();
double measureFuncDuration(int (*func_ptr)(void));

int main(int argc, char const *argv[])
{
    int e;
    double time;

    int n;

    double sumRead;
    double avgRead;

    double sumWrite;
    double avgWrite;

    if ((e = openDev(DEVICE)) != 0)
    {
        return e;
    }

    prepareData(SIZE/4);
    for (int i = 0; i < 10; i++)
    {
        measureFuncDuration(&writeToDev);
    }
    for (int i = 0; i < 10; i++)
    {
        measureFuncDuration(&readFromDev);
    }

    printf("\n");

    for (n = 0; n < 1; n++)
    {
        prepareData(n * SIZE);

        time = measureFuncDuration(&writeToDev);
        if (time != -1)
        {
            sumWrite = time;
            printf("Time to write: %lfus\n", time);
        }
        else
        {
            return time;
        }

        time = measureFuncDuration(&readFromDev);
        if (time != -1)
        {
            sumRead = time;
            printf("Time to read: %lfus\n", time);
        }
        else
        {
            return time;
        }
    }

    avgWrite = (sumWrite / n);
    avgRead = (sumRead / n);

    printf("\nN: %i\nSum write: %lf\nSum read: %lf\n", n, sumWrite, sumRead);

    printf("\nAvg write: %lf\n", avgWrite);
    printf("Avg read: %lf\n", avgRead);

    close(fd);
    free(dataToRead);
    return 0;
freeall:
    close(fd);
    free(dataToRead);
    return e;
}

int openDev(char *device)
{
    fd = open(DEVICE, O_RDWR);
    printf("fd: %i\n", fd);
    if (access(device, F_OK) == -1)
    {
        printf("Module %s not loaded... Close\n", device);
        return -2;
    }
    printf("Module %s loaded... \n", DEVICE);
    return 0;
}

void prepareData(int size)
{

    dataToWrite = (char *)malloc(size * sizeof(char));
    memset(dataToWrite, (char)65, sizeof(dataToWrite));

    dataToRead = (char *)malloc(size * sizeof(char));
    memset(dataToRead, 0, sizeof(dataToRead));
}

int writeToDev()
{
    ssize_t res;
    res = write(fd, &dataToWrite, sizeof(dataToWrite));
    if (res == -1)
    {
        printf("Error during write...\n");
        return -1;
    }
    printf("Zapisanych: %liB %s...\n", res, dataToWrite);
    return 0;
}

int readFromDev()
{
    ssize_t res;
    res = read(fd, &dataToRead, sizeof(dataToRead));
    if (res == -1)
    {
        printf("Error during read...\n");
        return -1;
    }
    printf("Precitaných: %liB %s... \n", res, dataToRead);
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
    {
        printf("Error in __FUNCTION__ = %s\n", __FUNCTION__);
        return -1;
    }
    return time_taken;
}
