#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


void handler(int sig, siginfo_t *info, void *ucontext) {

    printf("Signal number %d\n", info->si_signo);
    printf("ID of sending process %d\n", info->si_pid);
    printf("User id %d\n", info->si_uid);
}

void handler_nostop(int sig, siginfo_t *info, void *ucontext) {

    printf("Signal received\n");
}

void handler_nowait(int sig, siginfo_t *info, void *ucontext) {

    printf("Signal received\n");
}


int main(int argc, char **argv) {

    if (argc != 2) {
        perror("Wrong number of arguments");
    }

    if (strcmp(argv[1], "info") == 0) {
        struct sigaction act;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_SIGINFO;

        act.sa_sigaction = handler;
        sigaction(SIGCHLD, &act, NULL);

        if (fork() == 0) {
            exit(0);
        }

        wait(NULL);  
    }

    else if (strcmp(argv[1], "nostop")) { // do not get notification when child proc stop

        
        struct sigaction act;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_NOCLDSTOP;

        act.sa_sigaction = handler_nowait;
        sigaction(SIGCHLD, &act, NULL);

        if (fork() == 0) {
            exit(0);
        }

        wait(NULL);  
    
    }

    else if (strcmp(argv[1], "nowait")) { // do not change child to zobies when it terminates

        
        struct sigaction act;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_NOCLDWAIT;

        act.sa_sigaction = handler_nostop;
        sigaction(SIGCHLD, &act, NULL);

        if (fork() == 0) {
            exit(0);
        }

        wait(NULL);  
    
    }


    else {

        perror("Bad argument");
    }
}