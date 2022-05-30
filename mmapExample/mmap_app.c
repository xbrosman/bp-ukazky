#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
  
clock_t start, end;
double cpu_time_used;

#define DEVICE "/dev/mmap_example_module"
#define P_SIZE 4096
#define SIZE 4096
int fd = 0;
int offset = 0;
char *dataToWrite;

void prepareData(){
    dataToWrite = (char*)malloc(SIZE*sizeof(char));    
    int i;
    for (i=0; i < SIZE;i++)
    {
        dataToWrite[i] = 'A'+(char)(i%24);
    }
    dataToWrite[SIZE] = '\0';
}

int writeToDev() {
    char *p = NULL;
    fd = open(DEVICE, O_RDWR); 
    if(fd >= 0) { 
        p = (char*)mmap(0, P_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);  
        memcpy(p, dataToWrite, strlen(dataToWrite));   
        //printf("File descriptor %d, Writen: %ld\n", fd, strlen(p));       
        munmap(p, 4096);  
        close(fd);  
    }    
    return 0;
}

int readFromDev() {
    char *p = NULL;
    fd = open(DEVICE, O_RDWR); 
    if(fd >= 0) { 
        p = (char*)mmap(0, P_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);  
        //printf("File descriptor %d, read: %ld\n", fd, strlen(p));       
        close(fd);  
    }    
    return 0;
}

int main(int argc, char const *argv[])
{
    clock_t t;
    double time_taken;   

    if (access(DEVICE, F_OK) == -1){
    
        printf("Module %s not loaded... Close\n", DEVICE);
        return 0;
    }
    printf("Module %s loaded... \n", DEVICE);
    
    prepareData();
   
    t = clock();
    writeToDev();     
    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Cas na vykonanie zapisu: %fus\n", time_taken*1000000); 

    t = clock();
    readFromDev(); 
    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Cas na vykonanie citania: %fus\n", time_taken*1000000); 

    return 0;
}
