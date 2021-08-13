#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/times.h>
#include <time.h>

#define N_REINDEER 9
#define N_ELF 10

int elf_count;
int reindeer_count;

int time_gifts_given = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeer_condition = PTHREAD_COND_INITIALIZER;
pthread_cond_t elf_condition = PTHREAD_COND_INITIALIZER;

pthread_t elf_id[3];


void sleep_between(int from, int to) {

    struct timespec ts;
    ts.tv_sec = rand() % (to - from ) + to - 1;
    ts.tv_nsec = (rand() % 1000) * 1000000;

    nanosleep(&ts, NULL);
}

#define ELF_WORKING sleep_between(2, 5)
#define PROBLEM_SOLVING sleep_between(1,2)
#define HOLIDAYS sleep_between(5, 10)
#define GIVE_GIFTS sleep_between(2, 4)

void *santa() {

    while(1) {
        pthread_mutex_lock(&mutex);
        if((reindeer_count < 9) && (elf_count < 3)) {
            printf("MIKOLAJ: ZASYPIAM\n");
            pthread_cond_wait(&condition, &mutex);
            printf("MIKOLAJ: BUDZE SIE\n");
        }
        if(reindeer_count == 9) {
            printf("MIKOLAJ: DOSTARCZAM ZABAWKI\n");
            pthread_cond_broadcast(&reindeer_condition);
            GIVE_GIFTS;
            time_gifts_given++;
            reindeer_count = 0;
        }
        if (elf_count == 3) {
            printf("MIKOLAJ: ROZWIAZUJE PROBLEMY ELFOW, %ld, %ld, %ld\n", elf_id[0], elf_id[1], elf_id[2]);
            pthread_cond_broadcast(&elf_condition);
            PROBLEM_SOLVING;
            elf_count = 0;
        }
        pthread_mutex_unlock(&mutex);
        if (time_gifts_given == 2) {
            pthread_exit((void*) 0);
        }
    }
}

void *reindeer() {
    while(1) {
        HOLIDAYS;
        pthread_mutex_lock(&mutex);
        reindeer_count ++;
        printf("RENIFER: CZEKA %d RENIFEROW, %ld\n", reindeer_count, pthread_self());
        if (reindeer_count == 9) {
            printf("RENIFER: WYBUDZAM MIKOLAJA, %ld\n", pthread_self());
            pthread_cond_broadcast(&condition);
        }
        else {
            pthread_cond_wait(&reindeer_condition, &mutex);
        }
        pthread_mutex_unlock(&mutex);
        GIVE_GIFTS;
    }
}

void *elf() {
    while(1) {
        ELF_WORKING;
        pthread_mutex_lock(&mutex);
        if (elf_count < 3) {
            elf_id[elf_count] = pthread_self();
            elf_count ++;
            printf("ELF: CZEKA %d ELFOW NA MIKOLAJA %ld\n", elf_count, pthread_self());
            if (elf_count == 3) {
                printf("ELF: WYBUDZAM MIKOLAJA, %ld\n", pthread_self());
                pthread_cond_broadcast(&condition);
            }
            else {
                pthread_cond_wait(&elf_condition, &mutex);
            }
            pthread_mutex_unlock(&mutex);
            printf("ELF: MIKOLAJ ROZWIAZUJE PROBLEM, %ld\n", pthread_self());
            PROBLEM_SOLVING;
        }
        else{
            pthread_mutex_unlock(&mutex);
            printf("ELF: CZEKAM NA POWROT ELFOW, %ld\n", pthread_self());
        }
    }
}

int main(int argc, char** argv) {
    pthread_t *threads = calloc(N_ELF + N_REINDEER + 1, sizeof(pthread_t));

    pthread_create(&threads[0], NULL, santa, NULL);

    for(int i = 1; i <= N_REINDEER; ++i) {
        pthread_create(&threads[i], NULL, reindeer, NULL);
    }

    for(int i = N_REINDEER + 1; i <= N_REINDEER + N_ELF; ++i) {
        pthread_create(&threads[i], NULL, elf, NULL);
    }

    pthread_join(threads[0], NULL);
    exit(0);

}



