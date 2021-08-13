#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

int MAX_LINE_DIGITS = 2;

void error (char * msg) {
    printf("ERROR: %s\n", msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    char *pipe_path = argv[1];
    int line_number = atoi(argv[2]);
    char *file_path = argv[3];
    int n = atoi(argv[4]);

    srand(time(NULL));

    FILE *file = fopen(file_path, "r");

    int pipe_write = open(pipe_path, O_WRONLY);

    if (pipe_write < 0) {
        error("Cannot open pipe!");
    }

    char buffer[n];
    int product_length = n + MAX_LINE_DIGITS;

    while (fgets(buffer, n, file) != NULL) {
        char product[product_length];
        if(line_number/10 > 0) {
            sprintf(product, "%d||%s\n", line_number, buffer);
        }
        else {
            sprintf(product, "|%d||%s\n", line_number, buffer);
        }
        write(pipe_write, product, strlen(product));
        sleep(rand() % 2);
    }
    close(pipe_write);
    fclose(file);

    return 0;
}