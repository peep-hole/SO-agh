#include <stdlib.h>
#include <stdio.h>
#include "pizza.h"

sem_t *semaphores[NUMBER_OF_SEMAPHORES];
int shared_memory_id_oven;
int shared_memory_id_table;


int pizza;
int pizza_index;

void prepare_pizza() {

    pizza = RANDOM_PIZZA;
    printf("(%d %ld) Przygotowuje pizze: %d\n", getpid(), TIMESTAMP, pizza);
}

void place_pizza_in_oven() {

    int pizzas_number;

    sem_getvalue(semaphores[I_PIZZAS_IN_OVEN], &pizzas_number);

    while (pizzas_number == OVEN_CAPACITY) {
       sleep_between(0, 1);
       sem_getvalue(semaphores[I_PIZZAS_IN_OVEN], &pizzas_number);
   }

    sem_wait(semaphores[I_OVEN_PLACE]);

    sem_getvalue(semaphores[I_OVEN_INDEX], &pizza_index);

    pizza_index = pizza_index % OVEN_CAPACITY;

    int *pizzas_in_oven = mmap(NULL, OVEN_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_id_oven, 0);

    if (pizzas_in_oven == (void *)-1) {
        error("Cant map memory");
    }

    pizzas_in_oven[pizza_index] = pizza;

    int n_pizzas;

    sem_getvalue(semaphores[I_PIZZAS_IN_OVEN], &n_pizzas);

    if (munmap(pizzas_in_oven, OVEN_CAPACITY) < 0) {
        error("Can't unmap shared memory");
    }

    sem_post(semaphores[I_OVEN_INDEX]);
    sem_post(semaphores[I_PIZZAS_IN_OVEN]);
    sem_post(semaphores[I_OVEN_PLACE]);



    printf("(%d %ld) Dodalem pizze: %d. Liczba pizz w piecu: %d\n", getpid(), TIMESTAMP, pizza, n_pizzas + 1);

}

void get_pizza() {

    int pizzas_number;

    sem_getvalue(semaphores[I_PIZZAS_ON_TABLE], &pizzas_number);

    while (pizzas_number == TABLE_CAPACITY) {
        sleep_between(0, 1);
        sem_getvalue(semaphores[I_PIZZAS_ON_TABLE], &pizzas_number);
    }

    sem_wait(semaphores[I_OVEN_PLACE]);

    int *pizzas_in_oven = mmap(NULL, OVEN_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_id_oven, 0);

    if (pizzas_in_oven == (void *)-1) {
        error("Cant map memory");
    }

    pizzas_in_oven[pizza_index] = 0;

    if (munmap(pizzas_in_oven, OVEN_CAPACITY) < 0) {
        error("Can't unmap shared memory");
    }

    int table_index;

    sem_getvalue(semaphores[I_TABLE_INDEX], &table_index);

    table_index = table_index % TABLE_CAPACITY;

    int *pizzas_on_table = mmap(NULL, TABLE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_id_table, 0);

    if (pizzas_on_table == (void *)-1) {
        error("Cant map memory");
    }


    int n_pizzas_on_table;
    int n_pizzas_in_oven;

    sem_getvalue(semaphores[I_PIZZAS_ON_TABLE], &n_pizzas_on_table);
    sem_getvalue(semaphores[I_PIZZAS_IN_OVEN], &n_pizzas_in_oven);

    pizzas_on_table[table_index] = pizza;

    if (munmap(pizzas_on_table, OVEN_SIZE) < 0) {
        error("Can't unmap shared memory");
    }

    sem_post(semaphores[I_TABLE_INDEX]);
    sem_wait(semaphores[I_PIZZAS_IN_OVEN]);
    sem_post(semaphores[I_PIZZAS_ON_TABLE]);
    sem_post(semaphores[I_OVEN_PLACE]);

    printf("(%d %ld) Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", getpid(), TIMESTAMP, pizza, n_pizzas_in_oven - 1, n_pizzas_on_table + 1);
}




int main(int argc, char **argv) {
    srand(TIMESTAMP);

    get_semaphore(OVEN_PLACE, &semaphores[0]);
    get_semaphore(TABLE_PLACE_COURIER, &semaphores[1]);
    get_semaphore(PIZZAS_IN_OVEN, &semaphores[2]);
    get_semaphore(PIZZAS_ON_TABLE, &semaphores[3]);
    get_semaphore(OVEN_INDEX, &semaphores[4]);
    get_semaphore(TABLE_INDEX, &semaphores[5]);
    get_semaphore(TABLE_PICK_INDEX, &semaphores[6]);


    shared_memory_id_oven = get_shared_memory(SHARED_MEM_OVEN);
    shared_memory_id_table = get_shared_memory(SHARED_MEM_TABLE);

    while(1) {
        prepare_pizza();
        PREPARE_TIME;
        place_pizza_in_oven();
        PIZZA_BAKING_TIME;
        get_pizza();
    }

    return 0;
}

