# include "semaphore.h"

void error(char *msg) {
    printf("ERROR: %s\n", msg);
    exit(1);
}

int get_semaphore() {
    key_t sem_key = ftok(getenv("HOME"), SEMAPHORE_ID);
    int semaphore_id = semget(sem_key, 0, 0);
    if (semaphore_id < 0) {
        error("Can't get shared semaphore");
    }
    return semaphore_id;
}

int get_shared_memory_oven() {
    key_t shm_key = ftok(getenv("HOME"), SHARED_MEM_ID_OVEN);
    int shared_memory_id = shmget(shm_key, 0, 0);
    if (shared_memory_id < 0) {
        error("Can't get shared memory");
    }
    return shared_memory_id;
}

int get_shared_memory_table() {
    key_t shm_key = ftok(getenv("HOME"), SHARED_MEM_ID_TABLE);
    int shared_memory_id = shmget(shm_key, 0, 0);
    if (shared_memory_id < 0) {
        error("Can't get shared memory");
    }
    return shared_memory_id;
}

int create_semaphore() {
    key_t sem_key = ftok(getenv("HOME"), SEMAPHORE_ID);
    int semaphore_id = semget(sem_key, NUMBER_OF_SEMAPHORES, IPC_CREAT | 0666);
    if (semaphore_id < 0) {
        error("Cannot create semaphores");
    }
    union semun arg;
    arg.val = 0;

    for (int i = 0; i < 2; ++i) {
        semctl(semaphore_id, i, SETVAL, arg);
    }

    return semaphore_id;
}

int create_shared_memory_oven() {
    key_t shm_key = ftok(getenv("HOME"), SHARED_MEM_ID_OVEN);
    int shared_memory_id = shmget(shm_key, OVEN_SIZE, IPC_CREAT | 0666);
    if (shared_memory_id < 0) {
        error("Cannot create shared memory");
    }
    return shared_memory_id;
}

int create_shared_memory_table() {
    key_t shm_key = ftok(getenv("HOME"), SHARED_MEM_ID_TABLE);
    int shared_memory_id = shmget(shm_key, TABLE_SIZE, IPC_CREAT | 0666);
    if (shared_memory_id < 0) {
        error("Cannot create shared memory");
    }
    return shared_memory_id;
}

void sleep_between(int from, int to) {

    struct timespec ts;
    ts.tv_sec = rand() % (to - from ) + to - 1;
    ts.tv_nsec = (rand() % 1000) * 1000000;

    nanosleep(&ts, NULL);
}


