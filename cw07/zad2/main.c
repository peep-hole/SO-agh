#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include "pizza.h"



void unlink_sem(char *name) {

    if (sem_unlink(name) < 0) {
        error("Can't close semaphore");
    }
}

void unlink_mem(char *name) {

    if (shm_unlink(name)) {
        error("Can't close shared memory");
    }
}

void handler() {
    printf("\nClosing restaurant...\n");
    unlink_sem(OVEN_PLACE);
    unlink_sem(TABLE_PLACE_COURIER);
    unlink_sem(PIZZAS_IN_OVEN);
    unlink_sem(PIZZAS_ON_TABLE);
    unlink_sem(OVEN_INDEX);
    unlink_sem(TABLE_INDEX);
    unlink_sem(TABLE_PICK_INDEX);

    unlink_mem(SHARED_MEM_TABLE);
    unlink_mem(SHARED_MEM_OVEN);

    exit(0);
}

int main(int argc, char **argv) {

    if (argc != 3) {
        error("Wrong number of arguments");
    }

    int cooks = atoi(argv[1]);
    int couriers = atoi(argv[2]);

    signal(SIGINT, handler);
    create_semaphore(OVEN_PLACE, 1);
    create_semaphore(TABLE_PLACE_COURIER, 1);
    create_semaphore(PIZZAS_IN_OVEN, 0);
    create_semaphore(PIZZAS_ON_TABLE, 0);
    create_semaphore(OVEN_INDEX, 0);
    create_semaphore(TABLE_INDEX, 0);
    create_semaphore(TABLE_PICK_INDEX, 0);
    create_shared_memory(SHARED_MEM_OVEN);
    create_shared_memory(SHARED_MEM_TABLE);


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