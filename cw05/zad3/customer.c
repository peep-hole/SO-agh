#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int MAX_LINE_DIGITS = 2;

void error (char * msg) {
    printf("ERROR: %s\n", msg);
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

int get_num(const char * buffer) {

    int i;
    int cor = 0;
    if (buffer[0] == '|') {
        cor = 1;
    }
    for(i = 1; i < MAX_LINE_DIGITS + 1; ++i) {

        if (buffer[i] == '|') {
            break;
        }
    }

    char * number = (char *) calloc(i - cor, sizeof (char));

    for(int j = 0; j < i - cor; ++j) {
        number[j] = buffer[j + cor];
    }

    return atoi(number);
}

int main(int argc, char *argv[]) {
    char *pipe_path = argv[1];
    char *file_path = argv[2];
    int n = atoi(argv[3]);
    FILE *file = fopen(file_path, "r+");
    FILE *pipe_read = fopen(pipe_path, "r");
    if (pipe_read == NULL)  {
        error("Cannot read from pipe!");
    }
    char buffer[n];

    while (fgets(buffer, n, pipe_read) != NULL) {



        int length = 0;

        for (int i = MAX_LINE_DIGITS + 2; i < n; ++i) {
            if (buffer[i] == '\n'){
                length = i;
                break;
            }
        }

        char * tmp = (char *) calloc(length - MAX_LINE_DIGITS - 2, sizeof (char));

        for (int i = 0; i < length - 2 - MAX_LINE_DIGITS; ++i) {
            tmp[i] = buffer[i+2+MAX_LINE_DIGITS];
        }

        int line = get_num(buffer);

        rewind(file);

        for(int i = 0; i < line - 2; ++i) {
            char tmp_buf[50000];
            fgets(tmp_buf, 50000, file);
        }
        int c = getc(file);
        while((c != EOF)&&(c != '\n')) {
            c = getc(file);
        }
        fflush(file);

        fprintf(file,"%s", tmp);
        fflush(file);
    }


    fclose(file);
    fclose(pipe_read);
    return 0;
}