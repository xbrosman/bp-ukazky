/*
    bp_komunikacia/chardev_example/simple_app.c
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

#define DEVICE "/dev/simple_chardev"
#define SIZE 250*4096
int fd = 0;
int offset = 0;
char *dataToWrite;
char *dataToRead;

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
}

int writeToDev()
{
    ssize_t res;
    res = write(fd, dataToWrite, strlen(dataToWrite));
    if (res == -1){
        printf("Zapisovanie sa nepodarilo...\n");
        return res;
    }    
    return res;
}

int readFromDev()
{
    ssize_t res;
    res = read(fd, dataToRead, SIZE);
    if (res == -1){
        printf("citanie sa nepodarilo\n");
        return res;
    }    
    return res;
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
    fd = open(DEVICE, O_RDWR);
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
    free(dataToWrite);
    free(dataToRead);
    return 0;
freeall:
    close(fd);
    free(dataToWrite);
    free(dataToRead);
    return e;
}
