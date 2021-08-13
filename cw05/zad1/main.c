// assdfu
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int MAX_ARGUMENTS = 10;


struct Manuals {

    char *name;
    char ***manual;
    int manuals_count;

};

struct Commands {
    char** commands;
    int commands_count;
};



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

void read_lines(char **lines, char *file_name) {

    FILE *file = fopen(file_name, "r");

    char buffer[500];

    int i = 0;

    while (fgets(buffer, 500, file) != NULL) {

        strcpy(lines[i++], buffer);
    }
}

int find_break_index(char **lines, int lines_count) {

    int i;

    for(i = 0; i < lines_count; ++i) {
        if(lines[i][0] == '\n') break;

    }

    if(i == lines_count) error("No brake could be found");

    return i;

}

int command_to_sequence(char *command) {

    int start_index = 9;
    int i = start_index;
    while((command[i] != '\0')&&(command[i] != '\n')&&(command[i] != ' ')) {
        i++;
    }
    int length = i - start_index;

    char * number = (char*)calloc(length, sizeof(char));
    for (int j = 0; j < length; ++j) {
        number[j] = command[start_index + j];
    }
    int result = atoi(number);

    return result - 1;
}

int main(int argc, char **argv) {
    
    if(argc < 2) {

        error("Wrong number of arguments");
    }

    char *file_name = argv[1];

    int lines_number = count_lines(file_name);

    char **lines = (char **)calloc(lines_number + 1, sizeof(char *));

    for(int i = 0; i < lines_number + 1; ++i) {

        lines[i] = (char*)calloc(500, sizeof(char));
    }


    read_lines(lines, file_name);

    int break_index = find_break_index(lines, lines_number);

    struct Manuals *man_array = (struct Manuals*)calloc(break_index, sizeof(struct Manuals));

    
    ///////////////////////////////// PARSING COMMANDS (MANUALS) //////////////////////////////

    for(int i = 0; i < break_index; ++i) {

        int eq_index = 7;

        while(lines[i][eq_index] != '=') {
            eq_index++;
        }

        man_array[i].name = (char*)calloc(eq_index-1, sizeof(char));
        for (int j = 0; j < eq_index - 1; ++j) {
            man_array[i].name[j] = lines[i][j];
        }

        int j = eq_index + 2;
        char buffer[500];
        for(int k = 0; k < 500; ++k) {
            buffer[k] = '\0';
        }

        while (lines[i][j] != '\n') {

            buffer[j - eq_index- 2] = lines[i][j];
            j++;
        }
        buffer[j] = '\n';
        int commands_count = 0;

        for(int k = 0; k < j; ++k) {
            if(buffer[k] == '|') {
                commands_count++;
            }
        }
        commands_count++;

        man_array[i].manual = (char***)calloc(commands_count, sizeof(char**));
        man_array[i].manuals_count = commands_count;

        for (int k = 0; k < commands_count; ++k) {
            man_array[i].manual[k] = (char**)calloc(MAX_ARGUMENTS + 1, sizeof(char*));

            man_array[i].manual[k][0] = (char *) calloc(500, sizeof(char));
        }

        j = 0;
        int man_index = 0;
        int man_arr_index = 0;
        int arg_index = 0;

        while (buffer[j] != '\n') {

            if (buffer[j] == ' ') {
                man_index ++;
                if(buffer[j+1] != '|') {
                    man_array[i].manual[man_arr_index][man_index] = (char *) calloc(500, sizeof(char));
                }
                arg_index = 0;
                j += 1;
            }

            if (buffer[j] == '|') {
                man_arr_index++;
                man_index = 0;
                arg_index = 0;
                j += 2;
            }
            man_array[i].manual[man_arr_index][man_index][arg_index++] = buffer[j++];
        }
    }


    ///////////////////////////////////// PARSING COMMAND SEQUENCES ////////////////////////////////////////

    int command_sequence_count = lines_number - break_index - 2;

    struct Commands *commands_seq = (struct Commands *)calloc(command_sequence_count, sizeof(struct Commands));

    for (int i = break_index + 2; i < lines_number; ++i) {

        int commands_count = 0;
        int command_seq_index = i - break_index - 2;
        
        for(int j = 0; lines[i][j] != '\n'; ++j) {
            if(lines[i][j] == '|') {
                commands_count++;
            }
        }
        commands_count++;
        commands_seq[command_seq_index].commands_count = commands_count;
        commands_seq[command_seq_index].commands = (char **)calloc(commands_count, sizeof(char*));

        for (int j = 0; j < commands_count; ++j) {
            
            commands_seq[command_seq_index].commands[j] = (char *)calloc(100, sizeof(char));
        }

        int command_arr_index = 0;
        int command_index = 0;

        for(int j = 0; lines[i][j] != '\n'; ++j) {
            if(lines[i][j] == '|') {
                commands_seq[command_seq_index].commands[command_arr_index][command_index - 1] = '\0';
                command_arr_index++;
                command_index = 0;
                j += 2;

            }

            commands_seq[command_seq_index].commands[command_arr_index][command_index++] = lines[i][j];

        }
    }

    
    /////////////// EXECUTING COMMANDS //////////////////////

    for (int i = 0; i < command_sequence_count; ++i) {
        int total_man_count = 0;

        for (int j = 0; j < commands_seq[i].commands_count; ++j) {
            int index_c = command_to_sequence(commands_seq[i].commands[j]);

            total_man_count += man_array[index_c].manuals_count;
        }

        int **pipes = (int **) calloc(total_man_count, sizeof(int *));

        for (int j = 0; j < total_man_count; ++j) {
            pipes[j] = (int *) calloc(2, sizeof(int));

            if (pipe(pipes[j]) != 0) {

                error("Couldn't create a pipe");
            }
        }

        int pipe_index = 0;

        for (int j = 0; j < commands_seq[i].commands_count; ++j) {

            int index_c = command_to_sequence(commands_seq[i].commands[j]);

            for (int k = 0; k < man_array[index_c].manuals_count; ++k) {

                if (fork() == 0) {

                    if (pipe_index > 0) {
                        dup2(pipes[pipe_index - 1][0], STDIN_FILENO);
                    }

                    if (pipe_index < total_man_count - 1) {
                        dup2(pipes[pipe_index][1], STDOUT_FILENO);
                    }

                    for (int l = 0; l < total_man_count - 1; ++l) {
                        close(pipes[l][0]);
                        close(pipes[l][1]);
                    }


                    execvp(man_array[index_c].manual[k][0], man_array[index_c].manual[k]);
                    exit(0);
                }

                pipe_index += 1;
            }
        }

        for (int j = 0; j < total_man_count - 1; ++j) {
            close(pipes[j][0]);
            close(pipes[j][1]);
        }

        for (int j = 0; j < total_man_count; ++j) {
            wait(NULL);
        }
    }
    free(commands_seq);
    free(man_array);
    free(lines);


    return 0;
}