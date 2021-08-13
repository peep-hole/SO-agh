#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char **argv) {

    printf("__AFTER EXEC__\n\n");

    if (strcmp(argv[1], "pending") != 0)
    {
        raise(SIGUSR1);
    }
    if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0)
    {
        sigset_t mask;
        sigpending(&mask);

        char * msg;

        if (sigismember(&mask, SIGUSR1)) {
            msg = "Signal is pending";
        }

        else {
            msg = "Signal is not pending";
        }
        
        printf("%s\n", msg);
    }
    return 0;
}
