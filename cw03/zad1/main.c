#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include<sys/wait.h>


void error( char * msg) {

    printf("%s\n", msg);
    exit(1);
}


int main (int argc, char **argv) {

    if (argc != 2) {

        error("Niepoprawna liczba parametrow");
    }

    int n = atoi(argv[1]);

    printf("PID glownego programu: %d\n", (int)getpid());
    
    if(n == 0) return 0;

    for(int i = 0; i < n; ++i) {

        if(fork() == 0) {
            printf("    Proces %d pochodzi z procesu o id: %d\n",(int)getpid(), (int)getppid());
            exit(0);
        }
    }

    for(int i = 0; i < n; ++i) wait(NULL);

    return 0;
}