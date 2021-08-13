#ifndef SO_MYLIB_H
#define SO_MYLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct Line {
    char line[150];
};

struct BlockOfLines {
    struct Line ** lines;
    int number_of_lines;
    int is_used;
};

struct MainArray {
    struct BlockOfLines * blocks;
    int number_of_blocks;
    int blocks_used_count;
    int blocks_allocated_count;
};

struct Pair {
    char first[150];
    char second[150];
};

struct PairSequence {
    struct Pair * sequence;
    int size;
};



struct MainArray create_main_array(int size);

void merge_files(struct PairSequence pairSequence, struct MainArray *mainArray);

struct PairSequence create_pair_sequence(int size);

struct Pair init_pair(char * first_name, char * second_name);

void allocate_block_of_lines(struct MainArray *mainArray, int size);

int get_lines_count(struct MainArray mainArray, int block_index);

void remove_block_of_lines(struct MainArray *mainArray, int block_index);

void remove_line (struct MainArray mainArray, int block_index, int line_index);

void print_merged(struct MainArray mainArray);

void error(char * msg);




#endif //SO_MYLIB_H
