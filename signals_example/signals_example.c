/*
    bp_komunikacia/signals_example/signals_example.c
    Author: Filip Brosman
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Obsluha signálu
void handle_signal(int signal)
{
    printf("\nSignal: %d\n", signal);
    exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
    // Registracia obsluhy signálu
    signal(SIGINT, handle_signal);

    printf("Running, pid:%d\n", getpid());

    // Aplikácia beží bez prerušenia
    while (1)
    {
        sleep(1);
    }
    return 0;
}
