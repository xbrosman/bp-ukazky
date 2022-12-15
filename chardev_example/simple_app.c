/*
    bp_komunikacia/chardev_example/simple_app.c
    Author: Filip Brosman
*/
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>


#define DEBUG 1

clock_t start, end;
double cpu_time_used;

#define DEVICE "/dev/simple_chardev"
#define SIZE 256 * 4096     // 1MB
int fd = 0;
int offset = 0;
char *dataToWrite;
char *dataToRead;

void printLog(const char* format, ...);
int checkModule();
char* prepareDataWrite(size_t);
char* prepareDataRead(size_t);
void prepareData(size_t);
int writeToDev();
int readFromDev();
double measureFuncDuration(int (*func_ptr)(void));
int doMeasure();


int main(int argc, char const *argv[])
{
    int e;
    if (e = checkModule(DEVICE) != 0)
    {
        return e;
    }

    fd = open(DEVICE, O_RDWR);

    if (fd == -1)
    {
        printLog("Error in open file: %i\n", fd);
        return fd;
    }

    prepareData(SIZE);
    doMeasure();

    close(fd);
    free(dataToWrite);
    free(dataToRead);
    return 0;
}

int doMeasure()
{
    double time_taken = measureFuncDuration(writeToDev);    
    printLog("Data writen: %s\n", dataToWrite);
    if (time_taken < 0)
    {
        printLog("Error during reading.");
        return -1;
    }
    else
    {
        printf("Time to write: %fus\n", time_taken * 1000000);
    }

    time_taken = measureFuncDuration(readFromDev);
    printLog("Data read: %s\n", dataToRead);
    if (time_taken < 0)
    {
        printLog("Error during reading.");
        return -1;
    }
    else
    {
        printf("Time to read: %fus\n", time_taken * 1000000);
    }

    if (strcmp(dataToRead, dataToWrite) != 0)
    {
        printLog("Data writen and read are not equal");
        return -1;
    }
    return 0;
}

int checkModule(char *device)
{
    if (access(device, F_OK) == -1)
    {
        printLog("Module %s not loaded... Close\n", device);
        return -1; // module not loaded
    }
    printf("Module %s loaded... \n", device);
    return 0;
}

char* prepareDataWrite(size_t size)
{
    char* dataToWrite = (char *)malloc(size * sizeof(char) + 1);
    memset(dataToWrite, 65, size);
    dataToWrite[size] = '\0';
    return dataToWrite;
}

char* prepareDataRead(size_t size)
{
    char* dataToRead = (char *)malloc(size * sizeof(char));
    memset(dataToRead, 0, sizeof(dataToRead));
    return dataToRead;
}


void prepareData(size_t size)
{
    dataToWrite = prepareDataWrite(size);
    dataToRead = prepareDataRead(size);
}

int writeToDev()
{
    ssize_t res;
    res = write(fd, dataToWrite, strlen(dataToWrite), 0);
    if (res < 0)
    {
        printLog("Error in write!!!\n");
    }
    return res;
}

int readFromDev()
{
    ssize_t res;
    res = read(fd, dataToRead, SIZE, 0);
    if (res < 0)
    {
        printLog("Error in read!!!\n");
    }
    return res;
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
    if (e < 0)
    {
        printLog("Error in __FUNCTION__ = %s\n", __FUNCTION__);
        return e;
    }
    return time_taken;
}

void printLog(const char* format, ...)
{
    if(DEBUG){
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}
