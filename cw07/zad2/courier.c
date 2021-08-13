#include <stdlib.h>
#include <stdio.h>
#include "pizza.h"

sem_t *semaphores[NUMBER_OF_SEMAPHORES];
int shared_memory_id_table;


int pizza;

void give_pizza() {

    printf("(%d %ld) Dostarczam pizze: %d\n", getpid(), TIMESTAMP, pizza);
}

void pick_pizza() {

    int pizzas_number;

    sem_getvalue(semaphores[I_PIZZAS_ON_TABLE], &pizzas_number);

    while (pizzas_number == 0) {
        sleep_between(0, 1);
        sem_getvalue(semaphores[I_PIZZAS_ON_TABLE], &pizzas_number);
    }

    sem_wait(semaphores[I_TABLE_PLACE_COURIER]);


    int table_index;

    sem_getvalue(semaphores[I_TABLE_PICK_INDEX], &table_index);

    table_index = table_index % TABLE_CAPACITY;

    int *pizzas_on_table = mmap(NULL, TABLE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_id_table, 0);

    if (pizzas_on_table == (void *)-1) {
        error("Cant map memory");
    }

    int n_pizzas_on_table;

    sem_getvalue(semaphores[I_PIZZAS_ON_TABLE], &n_pizzas_on_table);

    pizza = pizzas_on_table[table_index];

    pizzas_on_table[table_index] = 0;

    if (munmap(pizzas_on_table, TABLE_SIZE) < 0) {
        error("Can't unmap shared memory");
    }

    sem_post(semaphores[I_TABLE_PICK_INDEX]);
    sem_wait(semaphores[I_PIZZAS_ON_TABLE]);
    sem_post(semaphores[I_TABLE_PLACE_COURIER]);

    printf(" (%d %ld) Pobieram pizze: %d Liczba pizz na stole: %d\n", getpid(), TIMESTAMP, pizza, n_pizzas_on_table - 1);
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

    shared_memory_id_table = get_shared_memory(SHARED_MEM_TABLE);

    while(1) {
        pick_pizza();
        DRIVING_TIME;
        give_pizza();
        DRIVING_TIME;
    }

    return 0;
}
