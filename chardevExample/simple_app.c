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

void prepareData(){
    dataToWrite = (char*)malloc(SIZE*sizeof(char));    
    int i;
    for (i=0; i < SIZE;i++)
    {
        dataToWrite[i] = 'A'+(char)(i%24);
    }
    dataToWrite[SIZE] = '\0';
}

void writeToDev() {
    fd = open(DEVICE, O_RDWR);
    char *dataToWrite = (char*)malloc(SIZE*sizeof(char));
   
    ssize_t res;      

    clock_t t;
    double time_taken;
    res = write(fd, dataToWrite, strlen(dataToWrite), &offset);

    if (res == -1)
        printf("Zapisovanie sa nepodarilo...\n");
    
   // printf("Zapisanych: %liB ...\n", res);
    free(dataToWrite);
    close(fd);
}

void readFromDev() {
    fd = open(DEVICE, O_RDWR);
    char *data = (char*)malloc(SIZE*sizeof(char));
    memset(data, 0, sizeof(data));
    ssize_t res;    

    
    res = read(fd, data, SIZE, &offset);
    
    if (res == -1)
        printf("citanie sa nepodarilo\n");
    //printf("Precitaných: %liB ... \n",res);
    free(data);
    close(fd);
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
