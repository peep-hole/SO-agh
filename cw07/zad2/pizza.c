# include "pizza.h"

void error(char *msg) {
    printf("ERROR: %s\n", msg);
    exit(1);
}

void get_semaphore(char * name, sem_t **sem) {
    *sem = sem_open(name, O_RDWR);
    if (*sem < 0) {
        error("Can't get semaphore");
    }
}


int get_shared_memory(char * name) {

    int result = shm_open(name, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if (result < 0) {
        error("Can't get shared memory");
    }

    return result;
}

void create_semaphore(char * name , int start_value) {

    sem_t *sem = sem_open(name, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, start_value);
    if (sem == SEM_FAILED) {
        error("Coud not create semaphore");
    }
    sem_close(sem);
}

void create_shared_memory(char * name) {

    int fd = shm_open(name, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd < 0) {
        error("Could not create shared memory");
    }
    if (ftruncate(fd, TABLE_SIZE) < 0) {
        error("Could not create memory of such size");
    }

}


void sleep_between(int from, int to) {

    struct timespec ts;
    ts.tv_sec = rand() % (to - from ) + to - 1;
    ts.tv_nsec = (rand() % 1000) * 1000000;

    nanosleep(&ts, NULL);
}


