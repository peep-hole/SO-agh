#ifndef CW07_SEMAPHOR_H
#define CW07_SEMAPHOR_H

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <time.h>


#define SEMAPHORE_ID 0
#define SHARED_MEM_ID_OVEN 1
#define SHARED_MEM_ID_TABLE 2
#define OVEN_CAPACITY 5
#define TABLE_CAPACITY 5
#define RANDOM_PIZZA rand() % 10
#define OVEN_SIZE (OVEN_CAPACITY * sizeof(int))
#define TABLE_SIZE (TABLE_CAPACITY * sizeof(int))
#define NUMBER_OF_SEMAPHORES 7

#define OVEN_PLACE 0
#define TABLE_PLACE_COURIER 1
#define PIZZAS_IN_OVEN 2
#define PIZZAS_ON_TABLE 3
#define OVEN_INDEX 4
#define TABLE_INDEX 5
#define TABLE_PICK_INDEX 6


union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

void sleep_between(int from, int to);

#define PREPARE_TIME sleep_between(1, 2)
#define PIZZA_BAKING_TIME sleep_between(4, 5)
#define DRIVING_TIME sleep_between(4, 5)
#define TIMESTAMP time(NULL)


void error(char * msg);
int get_semaphore();
int get_shared_memory_table();
int get_shared_memory_oven();
int create_semaphore();
int create_shared_memory_table();
int create_shared_memory_oven();


#endif //CW07_SEMAPHOR_H
