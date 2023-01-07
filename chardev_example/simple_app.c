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
#include <math.h>
#include <unistd.h>

#define DEBUG 0

clock_t start, end;
double cpu_time_used;

#define DEVICE "/dev/simple_chardev"
static size_t SIZE = 4096 ;    // 1MB
int fd = 0;
int offset = 0;
char *dataToWrite;
char *dataToRead;

void printLog(const char *format, ...);
void printErr(const char *format, ...);
int checkModule();
char *prepareDataWrite(size_t);
char *prepareDataRead(size_t);
int prepareData(size_t);

char *changeDataWriteSize(char*, size_t);
char *changeDataReadSize(char*, size_t);
int changeDataSize(size_t);

int writeToDev();
int readFromDev();
double measureFuncDuration(int (*func_ptr)(void));
int doMeasure();

int main(int argc, char const *argv[])
{
    int e;
    int sizes[5] = {1,128,256,512,1024};
    if (e = checkModule(DEVICE) != 0)
    {
        return e;
    }

    fd = open(DEVICE, O_RDWR);

    if (fd == -1)
    {
        printErr("Error in open file: %i\n", fd);
        return fd;
    }

    if (prepareData(SIZE) != 0)
    {
        printErr("Error in open file: %i\n", fd);
        return -1;
    }        

    for (int i = 0; i < 5; i++)
    {
        SIZE = sizes[i]*4096;
        printf("\nMeasurment number: %i, with data size: %liB \n", i+1, SIZE);
        if (changeDataSize(SIZE) != 0)
        {
            printErr("Error in open file: %i\n", fd);
            return -1;
        }
        doMeasure();  
    }
   

    close(fd);
    free(dataToWrite);
    free(dataToRead);
    return 0;
}

int doMeasure()
{
    double time_taken = 0;
   // warmUp(10);

    int n;
    double sumWrite = 0;
    double avgWrite = 0;

    double sumRead = 0;
    double avgRead = 0;

    for (n = 0; n < 100; n++)
    {
        time_taken = measureFuncDuration(writeToDev);
        sumWrite += time_taken;
   //     printLog("Data writen: %s\n", dataToWrite);
        if (time_taken < 0)
        {
            printLog("Error during reading.");
            return -1;
        }        
        printLog("Time to write: %fus\n", time_taken * 1000000);        
    }

    for (n = 0; n < 100; n++)
    {
        time_taken = measureFuncDuration(readFromDev);
        sumRead += time_taken;
     //   printLog("Data read: %s\n", dataToRead);
        if (time_taken < 0)
        {
            printLog("Error during reading.");
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

int checkModule(char *device)
{
    if (access(device, F_OK) == -1)
    {
        printErr("Module %s not loaded... Close\n", device);
        return -1; // module not loaded
    }
    printf("Module %s loaded... \n", device);
    return 0;
}

char *prepareDataWrite(size_t size)
{
    char* dataToWrite = (char *)malloc(size * sizeof(char));
    memset(dataToWrite, 65, size);
    dataToWrite[size+1] = '\0';
    return dataToWrite;
}

char *prepareDataRead(size_t size)
{
    char* dataToRead = (char *)malloc(size * sizeof(char));
    memset(dataToRead, 0, sizeof(dataToRead));
    return dataToRead;
}

int prepareData(size_t size)
{
    dataToWrite = prepareDataWrite(size);
    dataToRead = prepareDataRead(size);

    if (dataToWrite==NULL || dataToRead==NULL)
        return -1;

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
    memset(dataToRead, 0, sizeof(dataToRead));
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

int writeToDev()
{
    size_t res;
    res = write(fd, dataToWrite, strlen(dataToWrite));
    if (res <= 0)
    {
        printErr("Error in write!!!\n");
    }
    return res;
}

int readFromDev()
{
    size_t res;
    res = read(fd, dataToRead, SIZE);
    if (res <= 0)
    {
        printErr("Error in read!!!\n");
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
    printLog("ret: %li\n", e);
    if (e <= 0)
    {
        printErr("Error in __FUNCTION__ = %s\n", __FUNCTION__);
        return e;
    }
    return time_taken;
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
