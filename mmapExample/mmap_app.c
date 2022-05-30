#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
  
clock_t start, end;
double cpu_time_used;

#define DEVICE "/dev/mmap_example_module"
#define PAGE_SIZE 4096
int fd = 0;
int offset = 0;

int writeToDev() {
   
    return 0;
}

int readFromDev() {
    
    return 0;
}

int main(int argc, char const *argv[])
{
    clock_t t;
    double time_taken;

    char *address = NULL;

    if (access(DEVICE, F_OK) == -1){
    
        printf("Module %s not loaded... Close\n", DEVICE);
        return 0;
    }
    printf("Module %s loaded... \n", DEVICE);
    fd = open(DEVICE, O_RDWR);
    address = mmap (NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (address == MAP_FAILED)
    {
        perror ("mmap operation failed");
        return -1;
    }

    //printf ("Initial message: %s\n", address);
    memcpy (address + 11, "*user*", 6);
    //printf ("Changed message: %s\n", address);
    close (fd);

    return 0;
}