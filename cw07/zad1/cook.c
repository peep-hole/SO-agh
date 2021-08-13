#include <stdlib.h>
#include <stdio.h>
#include "semaphore.h"

int semaphore_id;
int shared_memory_id_oven;
int shared_memory_id_table;


int pizza;
int pizza_index;

void prepare_pizza() {

    pizza = RANDOM_PIZZA;
    printf("(%d %ld) Przygotowuje pizze: %d\n", getpid(), TIMESTAMP, pizza);
}

void place_pizza_in_oven() {

   while (semctl(semaphore_id, PIZZAS_IN_OVEN, GETVAL, NULL) == OVEN_CAPACITY) {
       sleep_between(0, 1);
   }

    struct sembuf *oven_place = calloc(2, sizeof(struct sembuf));
    oven_place[0].sem_num = OVEN_PLACE;
    oven_place[0].sem_op = 0;
    oven_place[0].sem_flg = 0;

    oven_place[1].sem_num = OVEN_PLACE;
    oven_place[1].sem_op = 1;
    oven_place[1].sem_flg = 0;

    semop(semaphore_id, oven_place, 2);

    pizza_index = semctl(semaphore_id, OVEN_INDEX, GETVAL, NULL) % OVEN_CAPACITY;

    int *pizzas_in_oven = shmat(shared_memory_id_oven, NULL, 0);

    pizzas_in_oven[pizza_index] = pizza;

    struct sembuf *pizza_baking = calloc(3, sizeof(struct sembuf));

    int n_pizzas = semctl(semaphore_id, PIZZAS_IN_OVEN, GETVAL, NULL);

    pizza_baking[0].sem_num = OVEN_INDEX;
    pizza_baking[0].sem_op = 1;
    pizza_baking[0].sem_flg = 0;

    pizza_baking[1].sem_num = PIZZAS_IN_OVEN;
    pizza_baking[1].sem_op = 1;
    pizza_baking[1].sem_flg = 0;

    pizza_baking[2].sem_num = OVEN_PLACE;
    pizza_baking[2].sem_op = -1;
    pizza_baking[2].sem_flg = 0;

    semop(semaphore_id, pizza_baking, 3);


    printf("(%d %ld) Dodalem pizze: %d. Liczba pizz w piecu: %d\n", getpid(), TIMESTAMP, pizza, n_pizzas + 1);

}

void get_pizza() {

    while (semctl(semaphore_id, PIZZAS_ON_TABLE, GETVAL, NULL) == TABLE_CAPACITY) {
        sleep_between(0, 1);
    }

    struct sembuf *oven_place = calloc(2, sizeof(struct sembuf));

    oven_place[0].sem_num = OVEN_PLACE;
    oven_place[0].sem_op = 0;
    oven_place[0].sem_flg = 0;

    oven_place[1].sem_num = OVEN_PLACE;
    oven_place[1].sem_op = 1;
    oven_place[1].sem_flg = 0;

    semop(semaphore_id, oven_place, 2);

    int *pizzas_in_oven = shmat(shared_memory_id_oven, NULL, 0);

    pizzas_in_oven[pizza_index] = 0;

    int table_index = semctl(semaphore_id, TABLE_INDEX, GETVAL, NULL) % TABLE_CAPACITY;

    int *pizzas_on_table = shmat(shared_memory_id_table, NULL, 0);

    int n_pizzas_on_table = semctl(semaphore_id, PIZZAS_ON_TABLE, GETVAL, NULL);

    int n_pizzas_in_oven = semctl(semaphore_id, PIZZAS_IN_OVEN, GETVAL, NULL);


    pizzas_on_table[table_index] = pizza;

    struct sembuf *pizza_ready = calloc(4, sizeof(struct sembuf));

    pizza_ready[0].sem_num = TABLE_INDEX;
    pizza_ready[0].sem_op = 1;
    pizza_ready[0].sem_flg = 0;

    pizza_ready[1].sem_num = PIZZAS_IN_OVEN;
    pizza_ready[1].sem_op = -1;
    pizza_ready[1].sem_flg = 0;

    pizza_ready[2].sem_num = PIZZAS_ON_TABLE;
    pizza_ready[2].sem_op = 1;
    pizza_ready[2].sem_flg = 0;

    pizza_ready[3].sem_num = OVEN_PLACE;
    pizza_ready[3].sem_op = -1;
    pizza_ready[3].sem_flg = 0;

    semop(semaphore_id, pizza_ready, 4);

    printf("(%d %ld) Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", getpid(), TIMESTAMP, pizza, n_pizzas_in_oven - 1, n_pizzas_on_table + 1);
}




int main(int argc, char **argv) {
    srand(TIMESTAMP);

    semaphore_id = get_semaphore();
    shared_memory_id_oven = get_shared_memory_oven();
    shared_memory_id_table = get_shared_memory_table();

    while(1) {
        prepare_pizza();
        PREPARE_TIME;
        place_pizza_in_oven();
        PIZZA_BAKING_TIME;
        get_pizza();
    }

    return 0;
}

