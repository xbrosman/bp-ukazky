#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
  
clock_t start, end;
double cpu_time_used;

#define DEVICE "/dev/simple_chardev"
#define SIZE 4096
int fd = 0;
int offset = 0;
char *dataToWrite;
char *dataToRead;

void prepareData(){
    dataToWrite = (char*)malloc(SIZE*sizeof(char));    
    int i;
    for (i=0; i < SIZE;i++)
    {
        dataToWrite[i] = 'A'+(char)(i%24);
    }
    dataToWrite[SIZE] = '\0';
    dataToRead = (char*)malloc(SIZE*sizeof(char));
    memset(dataToRead, 0, sizeof(dataToRead));

    fd = open(DEVICE, O_RDWR);
}

void writeToDev() {   
    ssize_t res;         
    res = write(fd, dataToWrite, strlen(dataToWrite), &offset);
    if (res == -1)
        printf("Zapisovanie sa nepodarilo...\n");    
   // printf("Zapisanych: %liB ...\n", res);   
}

void readFromDev() {
    ssize_t res;        
    res = read(fd, dataToRead, SIZE, &offset);    
    if (res == -1)
        printf("citanie sa nepodarilo\n");
    //printf("Precitaných: %liB ... \n",res);
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
    free(dataToWrite);
    free(dataToRead);
    return 0;
}
