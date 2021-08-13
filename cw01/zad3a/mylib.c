#include "mylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void error (char * msg) {
    printf("Error: %s\n", msg);
    exit(1);
}

int count_lines(const char * file_name) {
    int result = 0;

    FILE * file = fopen(file_name, "r");

    if (file == NULL) {

        char name[1000];

        strcpy(name, file_name);

        strcat(name, " <----- cannot open a file to count lines [count_lines]");

        error(name);
    }

    for (char c = getc(file); c != EOF; c = getc(file)) {
        if (c == '\n') {

            result++;
        }
    }

    fclose(file);

    return result;

}

int abs(int x) {

    if (x >= 0) return x;

    else return (-1) * x;
}

void add_line_to_block(struct MainArray mainArray, int block_index, int line_index, char * line_var) {

    if (mainArray.number_of_blocks < block_index) {

        error("block index out of range [add_line]");
    }

    if (mainArray.blocks[block_index].number_of_lines < line_index) {

        error("line index out of range [add_line]");
    }

    mainArray.blocks[block_index].lines[line_index] = (struct Line *) calloc (1, sizeof(struct Line));
    strcpy(mainArray.blocks[block_index].lines[line_index]->line, line_var);
}

void remove_line (struct MainArray mainArray, int block_index, int line_index) {

    if (mainArray.number_of_blocks < block_index) {

        error("block index out of range [remove_line]");
    }

    if (mainArray.blocks[block_index].number_of_lines < line_index) {

        error("line index out of range [remove_line]");
    }

    if (mainArray.blocks[block_index].lines[line_index] == NULL) {
        error("cannot remove non existing line [remove_line]");
    }

    free(mainArray.blocks[block_index].lines[line_index]);
    mainArray.blocks[block_index].lines[line_index] = NULL;
}

int get_first_free_block_index(struct MainArray mainArray) {

    for (int i = 0; i < mainArray.number_of_blocks; ++i) {

        if(mainArray.blocks[i].is_used == 0) return i;
    }

    error("No free block left [get_first_free_block]");

    return -1;
}

void allocate_block_of_lines(struct MainArray *mainArray, int size) {

    if (mainArray->blocks_allocated_count + size > mainArray->number_of_blocks) {

        error("More block allocated than max available [allocate_block_of_lines]");
    }

    mainArray->blocks_allocated_count += size;
}

int create_block_of_lines(struct MainArray *mainArray, int line_number) {

    if (mainArray->blocks_used_count >= mainArray->blocks_allocated_count) {

        error("Not enough blocks allocated [create_block_of_lines]");
    }

    int index = get_first_free_block_index(*mainArray);

    struct BlockOfLines block;

    block.lines = (struct Line **) calloc (line_number, sizeof(struct Line*));

    block.number_of_lines = line_number;

    block.is_used = 1;

    mainArray->blocks[index] = block;

    mainArray->blocks_used_count++;

    return index;
}

void remove_block_of_lines(struct MainArray *mainArray, int block_index) {

    if (block_index > mainArray->number_of_blocks) {
        error("block index out of range [remove_block]");
    }

    if (mainArray->blocks[block_index].is_used == 0) {

        error("cannot remove not used block of lines [remove_block_of_lines]");
    }

    for (int i = 0; i < mainArray->blocks[block_index].number_of_lines; ++i) {

        if (mainArray->blocks[block_index].lines[i] != NULL) {
            remove_line(*mainArray, block_index, i);
        }
    }

    free(mainArray->blocks[block_index].lines);

    mainArray->blocks[block_index].is_used = 0;
    mainArray->blocks_used_count --;

}

struct MainArray create_main_array(int size) {
    struct MainArray mainArray;

    mainArray.blocks = (struct BlockOfLines *) calloc(size, sizeof(struct BlockOfLines));

    mainArray.number_of_blocks = size;
    mainArray.blocks_allocated_count = 0;
    mainArray.blocks_used_count = 0;

    for(int i = 0; i < mainArray.number_of_blocks; ++i) {

        mainArray.blocks[i].is_used = 0;
    }

    return mainArray;
}

struct PairSequence create_pair_sequence(int size) {

    struct PairSequence pairSequence;

    pairSequence.sequence = (struct Pair *) calloc (size, sizeof(struct Pair));

    pairSequence.size = size;

    return pairSequence;
}

struct Pair init_pair(char * first_name, char * second_name) {

    struct Pair pair;

    strcpy(pair.first, first_name);
    strcpy(pair.second, second_name);

    return pair;
}

void remove_pair_sequence(struct PairSequence pairSequence) {

    free(pairSequence.sequence);
    pairSequence.size = 0;
}

int get_lines_count(struct MainArray mainArray, int block_index) {

    if (block_index > mainArray.number_of_blocks) {
        error("block index out of range [get_lines]");
    }

    return mainArray.blocks[block_index].number_of_lines;
}

void merge_pair(struct MainArray *mainArray, struct Pair pair) {

    FILE * tmp = fopen("tmp.txt", "a");
    if (tmp == NULL) {
        error("Cannot open tmp file to append [merge_pair]");
    }

    int first_lines_count = count_lines(pair.first);

    int second_lines_count = count_lines(pair.second);

    FILE * file1 = fopen(pair.first, "r");
    if (file1 == NULL) {

        error("Cannot open file1 to read [merge_pair]");
    }

    FILE * file2 = fopen(pair.second, "r");
    if (file2 == NULL) {

        error("Cannot open file2 to read [merge_pair]");
    }

    int index = create_block_of_lines(mainArray, first_lines_count + second_lines_count);

    int dif = abs(first_lines_count - second_lines_count);
    int eq_lines = first_lines_count + second_lines_count - dif;

    int is_first_bigger = 0;
    if (first_lines_count > second_lines_count) is_first_bigger = 1;

    for (int i = 0; i < eq_lines; ++i) {

        char buffer[150];

        if (i % 2 == 0) fgets(buffer, 150, file1);
        else fgets(buffer, 150, file2);

        add_line_to_block(*mainArray, index, i, buffer);
        fprintf(tmp, "%s", buffer);
    }

    for (int i = 0; i < dif; ++i) {

        char buffer[150];

        if (is_first_bigger) fgets(buffer, 150, file1);
        else fgets(buffer, 150, file2);

        add_line_to_block(*mainArray, index, i, buffer);
        fprintf(tmp, "%s", buffer);
    }

    fclose(tmp);
    fclose(file1);
    fclose(file2);


}

void merge_files(struct PairSequence pairSequence, struct MainArray * mainArray) {

    for (int i = 0; i < pairSequence.size; ++i) {
        merge_pair(mainArray, pairSequence.sequence[i]);
    }

}

void print_block(struct BlockOfLines blockOfLines) {
    if(blockOfLines.is_used == 1) {

        for (int i = 0; i < blockOfLines.number_of_lines; ++i) {

            if (blockOfLines.lines[i] != NULL) {

                printf("%s", blockOfLines.lines[i]->line);
            }
        }
    }
}

void print_merged(struct MainArray mainArray) {

    for (int i = 0; i < mainArray.number_of_blocks; ++i) {

        print_block(mainArray.blocks[i]);
    }
}