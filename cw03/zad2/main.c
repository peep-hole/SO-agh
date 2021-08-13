#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include<sys/wait.h>
#include "mylib.h"


double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}


int main (int argc, char **argv) {

    if (argc < 3)
        error("Not enaugh arguments");

    FILE * results = fopen("./raport/pomiar_zad_2.txt", "a");

    struct tms * tms_time[2];
    clock_t real_time[2];

    tms_time[0] = calloc(1, sizeof(struct tms *));
    tms_time[1] = calloc(1, sizeof(struct tms *));

    int number_of_pairs = atoi(argv[1]);

    struct PairSequence pairSequence[number_of_pairs];

    struct MainArray mainArray = create_main_array(number_of_pairs);

    allocate_block_of_lines(&mainArray, number_of_pairs);


    for (int j = 0; j < number_of_pairs; ++j) {

        pairSequence[j] = create_pair_sequence(1);

        char * newToken = strtok(argv[2+j], ":");
        char * first = newToken;
        newToken = strtok(NULL, ":");
        char * second = newToken;

        pairSequence[j].sequence[0] = init_pair(first, second);
    }

    real_time[0] = times(tms_time[0]);

    for(int i = 0; i < number_of_pairs; ++i) {

        if(fork() == 0) {
            merge_files(pairSequence[i], &mainArray);
            exit(0);        
        }
    }

    for(int i = 0; i < number_of_pairs; ++i) wait(NULL);

    real_time[1] = times(tms_time[1]);

            fprintf(results, "Times: Real = %lf, User = %lf, System = %lf \n\n",
                    calculate_time(real_time[0], real_time[1]),
                    calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime),
                    calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));

    fclose(results);

    return 0;
}