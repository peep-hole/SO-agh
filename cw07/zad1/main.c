#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include "semaphore.h"


int semaphore_id;
int shared_memory_id_table;
int shared_memory_id_oven;


void handler() {
    printf("\nClosing restaurant...\n");
    semctl(semaphore_id, 0, IPC_RMID, NULL);
    shmctl(shared_memory_id_oven, IPC_RMID, NULL);
    shmctl(shared_memory_id_table, IPC_RMID, NULL);
    exit(0);
}

int main(int argc, char **argv) {

    if (argc != 3) {
        error("Wrong number of arguments");
    }

    int cooks = atoi(argv[1]);
    int couriers = atoi(argv[2]);

    signal(SIGINT, handler);
    semaphore_id = create_semaphore();
    shared_memory_id_table = create_shared_memory_table();
    shared_memory_id_oven = create_shared_memory_oven();

    for (int i = 0; i < couriers; ++i) {
        if (fork() == 0) {
            printf("Hiring courier: %d\n", i + 1);
            execlp("./courier", "", NULL);
        }
        sleep_between(0, 1);

    }

    for (int i = 0; i < cooks; ++i) {
        if (fork() == 0) {
            printf("Hiring cook: %d\n", i + 1);
            execlp("./cook", "", NULL);
        }
        sleep_between(0, 1);

    }

    for (int i = 0; i < cooks + couriers; ++i) {
        wait(NULL);
    }

    handler();

    return 0;
}