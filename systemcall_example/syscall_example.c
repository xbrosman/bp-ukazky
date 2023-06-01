#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    pid_t pid;
    int ret;

    // vytvorenie noveho procesu a pid noveho procesu
    pid = fork();

    if (pid == 0)
    {
        // detsky proccess
        printf("Proccess: %d", getpid());
    }
    else if (pid > 0)
    {
        // rodičovsky proccess
        printf("Proccess: %d", getpid());
    }
    else
    {
        // Chyba pri vytvárani noveho procesu
        perror("Error: fork");
        return -1;
    }

    return 0;
}
