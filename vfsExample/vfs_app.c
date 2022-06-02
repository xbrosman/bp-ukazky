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

void prepareData(){
    dataToWrite = 1;    
    dataToRead = (char*)malloc(SIZE*sizeof(char));
    memset(dataToRead, 0, sizeof(dataToRead));

    fd = open(DEVICE, O_RDWR);
    printf("%i\n", fd);
}

void writeToDev() {   
    ssize_t res;         
    res = write(fd, &dataToWrite, sizeof(dataToWrite), &offset);
    if (res == -1)
        printf("Zapisovanie sa nepodarilo...\n");  
    printf("Zapisanych: %liB %i...\n", res, dataToWrite);   
}

void readFromDev() {
    ssize_t res;         
    res = read(fd, dataToRead, SIZE, &offset);    
    if (res == -1)
        printf("citanie sa nepodarilo\n");
    printf("Precitaných: %liB %i... \n",res, dataToRead);
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


    close(fd);
    free(dataToRead);
    return 0;
}
