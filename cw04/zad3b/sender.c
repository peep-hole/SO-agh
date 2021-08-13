#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>



int signal_to_receive;
int signal_to_end;

int mode;

int signals_to_send;
int already_send = 0; 
int signals_counter; 

union sigval val;
int catcher_id;

void handler(int sig, siginfo_t *info, void *ucontext)
{
    if (sig == signal_to_receive)
    {
        signals_counter++;

        if (already_send < signals_to_send) {


            already_send ++;
            if (mode == 3) {

                sigqueue(catcher_id, signal_to_receive, val);
            }

            else {

                kill(catcher_id, signal_to_receive);
            }

        }

        else {

            if (mode == 3) {

                sigqueue(catcher_id, signal_to_end, val);
            }

            else {

                kill(catcher_id, signal_to_end);
            }
        }




        if (mode == 2)
        {
            printf("number of received: %d, catcher index: %d\n", signals_counter, info->si_value.sival_int);
        }
    }
    else if (sig == signal_to_end)
    {

        printf("number received: %d, should be: %d\n", signals_counter, signals_to_send);
        exit(0);
    }
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        perror("Wrong number of arguments");
    }
    catcher_id = atoi(argv[1]);
    signals_to_send = atoi(argv[2]);

    char *sig_mode = argv[3];

    if (strcmp("kill", sig_mode) == 0) {
        mode = 1;
    }
    else if (strcmp("queue", sig_mode) == 0) {
        mode = 2;
    }
    else if (strcmp("sigrt", sig_mode) == 0)
    {
        mode = 3;
    }
    else {
        perror("Bad argument");
        exit(1);
    }

    if (mode == 3) {

        signal_to_receive = SIGRTMIN + 1;
        signal_to_end = SIGRTMIN + 2;
    }

    else {

        signal_to_receive = SIGUSR1;
        signal_to_end = SIGUSR2;
    }

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, signal_to_receive);
    sigdelset(&mask, signal_to_end);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
        perror("cant block signals\n");
        exit(1);
    }

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, signal_to_receive);
    sigaddset(&act.sa_mask, signal_to_end);
    sigaction(signal_to_receive, &act, NULL);
    sigaction(signal_to_end, &act, NULL);

    printf("Sender PID: %d\n", getpid());

    if (mode == 1 || mode == 3) {
        already_send ++;
        kill(catcher_id, signal_to_receive);
    }
    else {
        
        val.sival_int = 0;
        already_send ++;
        sigqueue(catcher_id, signal_to_receive, val);
    }

    while (1) {
        sleep(1000);
    }

    return 0;
}