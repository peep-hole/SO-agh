#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


void error (char * msg) {
    printf("Error: %s\n", msg);
    exit(1);
}

void handler(int signal) {

    printf("Signal received\n\n");
}

int main (int argc, char **argv) {

    int is_exec = 0;
    if (argc != 2) {

        if (argc == 3) {
            is_exec = 1;
        }
        else {
            
            error("Wrong number of arguments passed");
        }
    }


    if (strcmp(argv[1], "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);

        raise(SIGUSR1);
        
        if (is_exec) {
            execl("./second", "./second", argv[1], NULL);
            
        }

        else if (fork()==0) {

            printf("__AFTER FORK__\n\n");

            raise(SIGUSR1);
            exit(0);
        }

        wait(NULL);

    }

    else if (strcmp(argv[1], "handler") == 0){
        signal(SIGUSR1, handler);

        if (is_exec) {
            execl("./second", "./second", argv[1], NULL);
            
        }

        else if (fork()==0) {

            printf("__AFTER FORK___\n\n");

            raise(SIGUSR1);
            exit(0);
        }

        wait(NULL);
    }

    else if (strcmp(argv[1], "mask") == 0){
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);

        if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
            
            error("Signal blocking failed");
        }

        raise(SIGUSR1);

        char * msg;

        if (sigismember(&mask, SIGUSR1)) {
            msg = "Signal is pending";
        }

        else {
            msg = "Signal is not pending";
        }
        
        printf("%s\n\n", msg);

        if (is_exec) {
            execl("./second", "./second", argv[1], NULL);
            
        }

        else if (fork()==0) {

            printf("__AFTER FORK__\n\n");

            raise(SIGUSR1);
            sigpending(&mask);

            char * msg2;

            if (sigismember(&mask, SIGUSR1)) {
            msg2 = "Signal is pending";
            }

            else {
                msg2 = "Signal is not pending";
            }
        
            printf("%s\n", msg2);
            
            exit(0);
        }

        wait(NULL);
    }

    else if (strcmp(argv[1], "pending") == 0) {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);

        if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
            
            error("Signal blocking failed");
        }

        raise(SIGUSR1);
        
        char * msg;

        if (sigismember(&mask, SIGUSR1)) {
            msg = "Signal is pending";
        }

        else {
            msg = "Signal is not pending";
        }
        
        printf("%s\n\n", msg);

        if (is_exec) {
            execl("./second", "./second", argv[1], NULL);
            
        }

        else if (fork()==0) {

            printf("__AFTER FORK__\n\n");

            sigpending(&mask);

            char * msg2;

            if (sigismember(&mask, SIGUSR1)) {
            msg2 = "Signal is pending";
            }

            else {
                msg2 = "Signal is not pending";
            }
        
            printf("%s\n", msg2);
            
            exit(0);
        }

        wait(NULL);
    }

    else
        error("Bad argument");


    return 0;
}
