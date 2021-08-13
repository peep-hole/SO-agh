#include "mylib.h"
#include <ctype.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <dlfcn.h>

int is_number (char* s) {
    for (int i = 0; i < strlen(s); i++) {

        if (!isdigit(s[i])) {

            return 0;
        }

        if ((i == 1) && (s[i-1] == '0')) {

            return 0;
        }
    }
    return 1;
}

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

void delete_main_array(struct MainArray * mainArray) {

    void *handle = dlopen("./libmylib.so",RTLD_LAZY);
    if(!handle){
        error("Open error");
    }

    void (* remove_block_of_lines)() = (void (*)())dlsym(handle, "remove_block_of_lines");

    for (int i = 0; i < mainArray->number_of_blocks; ++i) {

        if (mainArray->blocks[i].is_used == 1) {

            remove_block_of_lines(mainArray, i);
        }
    }

    dlclose(handle);

    mainArray->number_of_blocks = 0;
    mainArray->blocks_used_count = 0;
    mainArray->blocks_allocated_count = 0;
}



int main (int argc, char **argv) {

    void *handle = dlopen("./libmylib.so",RTLD_LAZY);
    if(!handle){
        error("Open error");
    }

//    struct ArrayOfBlocks (*createArrayOfBlocks)() = (struct ArrayOfBlocks (*)())dlsym(handle, "createArrayOfBlocks");

    struct MainArray (* create_main_array)() = (struct MainArray (*)())dlsym(handle, "create_main_array");

    void (* allocate_block_of_lines)() = (void (*)())dlsym(handle, "allocate_block_of_lines");

    struct PairSequence (* create_pair_sequence)() = (struct PairSequence (*)())dlsym(handle, "create_pair_sequence");

    void (* error)() = (void (*)())dlsym(handle, "error");

    struct Pair (* init_pair)() = (struct Pair (*)())dlsym(handle, "init_pair");

    void (* merge_files)() = (void (*)())dlsym(handle, "merge_files");

    void (* remove_block_of_lines)() = (void (*)())dlsym(handle, "remove_block_of_lines");

    void (* remove_line)() = (void (*)())dlsym(handle, "remove_line");




    FILE * resultFile = fopen("./raport/raport3b.txt", "a");

    if (argc < 2)
        error("Not enough arguments passed");

    int size = atoi(argv[1]);

    struct MainArray mainArray = create_main_array(size);

    struct tms * tms_time[2];
    clock_t real_time[2];

    tms_time[0] = calloc(1, sizeof(struct tms *));
    tms_time[1] = calloc(1, sizeof(struct tms *));

    for (int i = 2; i < argc; ++i){

        if (strcmp(argv[i], "create_table") == 0) {

            if (i + 1 >= argc) {
                error("Too less arguments");
            }

            if (!is_number(argv[i+1]))
                error("Argument should be a number");

            printf("Create table size %s\n", argv[i+1]);
            real_time[0] = times(tms_time[0]);

            allocate_block_of_lines(&mainArray, atoi(argv[i + 1]));

            real_time[1] = times(tms_time[1]);

            printf("Times: Real = %lf, User = %lf, System = %lf \n\n",
                   calculate_time(real_time[0], real_time[1]),
                   calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime),
                   calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));
            fprintf(resultFile, "Create table size %s\n", argv[i+1]);
            fprintf(resultFile, "Times: Real = %lf, User = %lf, System = %lf \n\n",
                    calculate_time(real_time[0], real_time[1]),
                    calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime),
                    calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));

            i += 1;
        }

        else if (strcmp(argv[i], "merge_files") == 0){

            if (i + 1 >= argc) {
                error("Too less arguments");
            }

            int arg_count = atoi(argv[i+1]);

            if (i + 1 + arg_count >= argc) {
                error("wrong number of argument passed");
            }

            real_time[0] = times(tms_time[0]);

            i += 2;

            struct PairSequence pairSequence;
            pairSequence = create_pair_sequence(arg_count);

            for (int j = 0; j < arg_count; ++j) {

                char * newToken = strtok(argv[i+j], ":"); // todo carefully
                char * first = newToken;
                newToken = strtok(NULL, ":");
                char * second = newToken;

                pairSequence.sequence[j] = init_pair(first, second);

            }

            merge_files(pairSequence, &mainArray);

            real_time[1] = times(tms_time[1]);

            printf("Merging %s pairs of files\n", argv[i - 1]);
            printf("Times: Real = %lf, User = %lf, System = %lf \n\n",
                   calculate_time(real_time[0], real_time[1]),
                   calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime),
                   calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));
            fprintf(resultFile, "Merging %s pairs of files\n", argv[i - 1]);
            fprintf(resultFile, "Times: Real = %lf, User = %lf, System = %lf \n\n",
                    calculate_time(real_time[0], real_time[1]),
                    calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime),
                    calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));

            i += arg_count - 1;

        }

        else if (strcmp(argv[i], "remove_block") == 0){

            if (i + 1 >= argc) {
                error("Too less arguments");
            }

            if (!is_number(argv[i + 1])) {

                error("Argument should be a number");;
            }

            real_time[0] = times(tms_time[0]);

            remove_block_of_lines(&mainArray, atoi(argv[i]));

            real_time[1] = times(tms_time[1]);

            printf("Removing block number %s\n", argv[i + 1]);
            printf("Times: Real = %lf, User = %lf, System = %lf \n\n",
                   calculate_time(real_time[0], real_time[1]),
                   calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime),
                   calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));
            fprintf(resultFile, "Removing block number %s\n", argv[i + 1]);
            fprintf(resultFile, "Times: Real = %lf, User = %lf, System = %lf \n\n",
                    calculate_time(real_time[0], real_time[1]),
                    calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime),
                    calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));

            i += 1;
        }

        else if (strcmp(argv[i], "remove_row") == 0) {

            if (i + 2 >= argc) {
                error("Too less arguments");
            }

            if ( ( !is_number(argv[i + 1]) ) || ( !is_number(argv[i + 2]) ) ) {

                error("Argument should be a number");;
            }

            real_time[0] = times(tms_time[0]);

            remove_line(mainArray, atoi(argv[i+1]), atoi(argv[i+2]));

            real_time[1] = times(tms_time[1]);

            printf("Removing line number %s from block number %s\n",argv[i + 2], argv[i + 1]);
            printf("Times: Real = %lf, User = %lf, System = %lf \n\n",
                   calculate_time(real_time[0], real_time[1]),
                   calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime),
                   calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));
            fprintf(resultFile, "Removing line number %s from block number %s\n",argv[i + 2], argv[i + 1]);
            fprintf(resultFile, "Times: Real = %lf, User = %lf, System = %lf \n\n",
                    calculate_time(real_time[0], real_time[1]),
                    calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime),
                    calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));

            i += 2;

        }

        else
            error("Bad argument");
    }

    delete_main_array(&mainArray);

    dlclose(handle);

    fclose(resultFile);

    return 0;
}