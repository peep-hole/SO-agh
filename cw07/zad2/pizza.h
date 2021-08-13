#ifndef CW07_PIZZA_H
#define CW07_PIZZA_H

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


#define SHARED_MEM_OVEN "/SHARED_MEM_OVEN"
#define SHARED_MEM_TABLE "/SHARED_MEM_TABLE"
#define OVEN_CAPACITY 5
#define TABLE_CAPACITY 5
#define RANDOM_PIZZA rand() % 10
#define OVEN_SIZE (OVEN_CAPACITY * sizeof(int))
#define TABLE_SIZE (TABLE_CAPACITY * sizeof(int))
#define NUMBER_OF_SEMAPHORES 7

#define OVEN_PLACE "/OVEN_PLACE"
#define TABLE_PLACE_COURIER "/TABLE_PLACE_COURIER"
#define PIZZAS_IN_OVEN "/PIZZAS_IN_OVEN"
#define PIZZAS_ON_TABLE "/PIZZAS_ON_TABLE"
#define OVEN_INDEX "/OVEN_INDEX"
#define TABLE_INDEX "/TABLE_INDEX"
#define TABLE_PICK_INDEX "/TABLE_PICK_INDEX"

#define I_OVEN_PLACE 0
#define I_TABLE_PLACE_COURIER 1
#define I_PIZZAS_IN_OVEN 2
#define I_PIZZAS_ON_TABLE 3
#define I_OVEN_INDEX 4
#define I_TABLE_INDEX 5
#define I_TABLE_PICK_INDEX 6


void sleep_between(int from, int to);

#define PREPARE_TIME sleep_between(1, 2)
#define PIZZA_BAKING_TIME sleep_between(4, 5)
#define DRIVING_TIME sleep_between(4, 5)
#define TIMESTAMP time(NULL)



void error(char * msg);
void get_semaphore(char * name, sem_t **sem);
int get_shared_memory(char * name);
void create_semaphore(char * name, int start_value);
void create_shared_memory(char * name);


#endif //CW07_PIZZA_H
