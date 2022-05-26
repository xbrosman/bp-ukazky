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

int writeToDev() {
    char *dataToWrite = (char*)malloc(SIZE*sizeof(char));
   
    ssize_t ret;   
    int i;
    for (i=0; i < SIZE;i++)
    {
        dataToWrite[i] = 'A'+(char)(i%24);
    }
    
    int size = strlen(dataToWrite);   
    //printf("\nData na zapis: %s\n", dataToWrite);

    ret = write(fd, dataToWrite, size, &offset);

    if (ret == -1)
        printf("Zapisovanie sa nepodarilo...\n");
    
    printf("Zapisanych: %liB ...\n", ret);
    free(dataToWrite);
    return 0;
}

int readFromDev() {
    char *data = (char*)malloc(SIZE*sizeof(char));
    memset(data, 0, sizeof(data));
    data[0] = '\0';
    ssize_t ret;     

    ret = read(fd, data, SIZE, &offset);
    //printf("\nCitanie z  %s: %s\n", DEVICE, data);
    if (ret == -1)
        printf("citanie sa nepodarilo\n");
    printf("Precitaných: %liB ... \n",ret);
    free(data);
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

    fd = open(DEVICE, O_RDWR);    
    t = clock();
    writeToDev();
    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Cas na vykonanie zapisu: %fus\n", time_taken*1000000);
    
    close(fd);

    fd = open(DEVICE, O_RDWR);
   
    t = clock();
     readFromDev();
    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Cas na vykonanie citania: %fus\n", time_taken*1000000);
    close(fd);

    return 0;
}
