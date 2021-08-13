#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    char *producer1[] = {"./producer", "pipe", "14", "./prod1/file.txt", "10", NULL};
    char *producer2[] = {"./producer", "pipe", "7", "./prod2/file.txt", "10", NULL};
    char *producer3[] = {"./producer", "pipe", "22", "./prod3/file.txt", "10", NULL};
    char *customer[] = {"./customer", "pipe", "./cons/file.txt", "18", NULL};


    mkfifo("pipe", S_IRUSR | S_IWUSR);

    if (fork() == 0) {
        execvp(producer1[0], producer1);
    }

    if (fork() == 0) {
        execvp(producer2[0], producer2);
    }

    if (fork() == 0) {
        execvp(producer3[0], producer3);
    }

    if (fork() == 0) {
        execvp(customer[0], customer);
    }

    for (int i = 0; i < 6; i++)
        wait(NULL);

    return 0;
}


