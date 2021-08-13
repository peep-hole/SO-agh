#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>




int signal_to_recive;
int signal_to_end;
int mode; 
int signals_counter = 0;

void handler(int sig, siginfo_t *info, void *ucontext)
{
    if (sig == signal_to_recive)
        signals_counter++;
    else if (sig == signal_to_end)
    {
        if (mode == 1 || mode == 3)
        {
            for (int i = 0; i < signals_counter; ++i)
            {
                kill(info->si_pid, signal_to_recive);
            }
            kill(info->si_pid, signal_to_end);
        }
        else
        {
            union sigval value;
            for (int i = 0; i < signals_counter; ++i)
            {
                value.sival_int = i;
                sigqueue(info->si_pid, signal_to_recive, value);
            }
            sigqueue(info->si_pid, signal_to_end, value);
        }
        printf("CATCHER: received %d signals\n", signals_counter);
        exit(0);
    }
}

int main(int argc, char **argv) {

    if (argc != 2)
    {
        fprintf(stderr, "wrong arguments, usage: [mode]\n");
        exit(1);
    }

    char *sig_mode = argv[1];

    if (strcmp("kill", sig_mode) == 0) {
        mode = 1;
    }
    
    else if (strcmp("queue", sig_mode) == 0) {
        mode = 2;
    }

    else if (strcmp("sigrt", sig_mode) == 0) {
        mode = 3;
        
    }

    else {
        perror("Bad argument");
        exit(1);
    }

    if(mode == 3) {

        signal_to_recive = SIGRTMIN + 1;
        signal_to_end = SIGRTMIN + 2;
    }

    else {

        signal_to_recive = SIGUSR1;
        signal_to_end = SIGUSR2;
    }

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, signal_to_recive);
    sigdelset(&mask, signal_to_end);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
    {
        perror("could not block signal\n");
        exit(1);
    }

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler;

    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, signal_to_recive);
    sigaddset(&act.sa_mask, signal_to_end);

    sigaction(signal_to_recive, &act, NULL);
    sigaction(signal_to_end, &act, NULL);

    printf("Catcher PID: %d\n", getpid());

    // if (mode != 2) {

    //     sigsuspend(&mask);
    // } 
    

    // else {
        while(1) {

            sleep(100);
        }
    
    // }
        

    return 0;
}