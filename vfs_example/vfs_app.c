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
    if (res == -1){
        printf("Error during write()...\n");
        return 1;
    }
    //printf("Zapisanych: %liB %i...\n", res, dataToWrite);
    return 0;
}

int readFromDev()
{
    ssize_t res;
    res = read(fd, dataToRead, SIZE, &offset);
    if (res == -1){
        printf("Error during read()...\n");
        return 1;
    }
    //printf("Precitaných: %liB %i... \n", res, dataToRead);
    return 0;
}

int main(int argc, char const *argv[])
{
    clock_t t;
    double time_taken;
    int e = 0;

    if (access(DEVICE, F_OK) == -1)
    {

        printf("Module %s not loaded... Close\n", DEVICE);
        e = 1;
        goto freeall;
    }
    printf("Module %s loaded... \n", DEVICE);
    prepareData();

    t = clock();
    e = writeToDev();
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;
    if (e)
    {
        printf("Error during reading.");
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

    close(fd);
    free(dataToRead);
    return 0;
freeall:
    close(fd);
    free(dataToRead);
    return e;
}
