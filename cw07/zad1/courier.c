#include <stdlib.h>
#include <stdio.h>
#include "semaphore.h"

int semaphore_id;
int shared_memory_id_table;


int pizza;

void give_pizza() {

    printf("(%d %ld) Dostarczam pizze: %d\n", getpid(), TIMESTAMP, pizza);
}

void pick_pizza() {

    while (semctl(semaphore_id, PIZZAS_ON_TABLE, GETVAL, NULL) == 0) {
        sleep_between(0, 1);
    }

    struct sembuf *oven_place = calloc(2, sizeof(struct sembuf));

    oven_place[0].sem_num = TABLE_PLACE_COURIER;
    oven_place[0].sem_op = 0;
    oven_place[0].sem_flg = 0;

    oven_place[1].sem_num = TABLE_PLACE_COURIER;
    oven_place[1].sem_op = 1;
    oven_place[1].sem_flg = 0;

    semop(semaphore_id, oven_place, 2);

    int table_index = semctl(semaphore_id, TABLE_PICK_INDEX, GETVAL, NULL) % TABLE_CAPACITY;

    int *pizzas_on_table = shmat(shared_memory_id_table, NULL, 0);

    int n_pizzas_on_table = semctl(semaphore_id, PIZZAS_ON_TABLE, GETVAL, NULL);

    pizza = pizzas_on_table[table_index];

    pizzas_on_table[table_index] = 0;

    struct sembuf *pizza_ready = calloc(3, sizeof(struct sembuf));

    pizza_ready[0].sem_num = TABLE_PICK_INDEX;
    pizza_ready[0].sem_op = 1;
    pizza_ready[0].sem_flg = 0;

    pizza_ready[1].sem_num = PIZZAS_ON_TABLE;
    pizza_ready[1].sem_op = -1;
    pizza_ready[1].sem_flg = 0;

    pizza_ready[2].sem_num = TABLE_PLACE_COURIER;
    pizza_ready[2].sem_op = -1;
    pizza_ready[2].sem_flg = 0;

    semop(semaphore_id, pizza_ready, 3);

    printf(" (%d %ld) Pobieram pizze: %d Liczba pizz na stole: %d\n", getpid(), TIMESTAMP, pizza, n_pizzas_on_table - 1);
}




int main(int argc, char **argv) {
    srand(TIMESTAMP);

    semaphore_id = get_semaphore();
    shared_memory_id_table = get_shared_memory_table();

    while(1) {
        pick_pizza();
        DRIVING_TIME;
        give_pizza();
        DRIVING_TIME;
    }

    return 0;
}
