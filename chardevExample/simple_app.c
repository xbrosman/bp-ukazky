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

void writeToDev() {
    char *dataToWrite = (char*)malloc(SIZE*sizeof(char));
   
    ssize_t res;   
    int i;
    for (i=0; i < SIZE;i++)
    {
        dataToWrite[i] = 'A'+(char)(i%24);
    }
    
    int size = strlen(dataToWrite);   
    //printf("\nData na zapis: %s\n", dataToWrite);

    clock_t t;
    double time_taken;

    t = clock();
    res = write(fd, dataToWrite, size, &offset);
    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Cas na vykonanie zapisu: %fus\n", time_taken*1000000);

    if (res == -1)
        printf("Zapisovanie sa nepodarilo...\n");
    
    printf("Zapisanych: %liB ...\n", res);
    free(dataToWrite);
}

void readFromDev() {
    char *data = (char*)malloc(SIZE*sizeof(char));
    memset(data, 0, sizeof(data));
    ssize_t res;     

    clock_t t;
    double time_taken;
    t = clock();
    res = read(fd, data, SIZE, &offset);
    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Cas na vykonanie citania: %fus\n", time_taken*1000000);
    //printf("\nCitanie z  %s: %s\n", DEVICE, data);
    if (res == -1)
        printf("citanie sa nepodarilo\n");
    printf("Precitaných: %liB ... \n",res);
    free(data);
}

int main(int argc, char const *argv[])
{
    if (access(DEVICE, F_OK) == -1){
    
        printf("Module %s not loaded... Close\n", DEVICE);
        return 0;
    }
    printf("Module %s loaded... \n", DEVICE);

    fd = open(DEVICE, O_RDWR);    
    writeToDev();  
    close(fd);

    fd = open(DEVICE, O_RDWR);  
    readFromDev();
    close(fd);

    return 0;
}
