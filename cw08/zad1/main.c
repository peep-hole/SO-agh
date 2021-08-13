#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define TEST test(row, col, num);

void test(int row, int col, char * num) {
    printf("test row: %d, col: %d, num %s\n", row, col, num);
}

int W, H, threads;
int **image;
int **result_image;

int BUFFER_SIZE = 256;

void error(char *msg) {
    printf("ERROR: %s\n", msg);
    exit(1);
}

struct thread_arg {
    char* mode;
    int thread_n;
};

void get_size(char *buffer) {
    char *w;
    char *h;
    char *tok = buffer;

    w = strtok_r(tok, " \t\r\n", &tok);
    h = strtok_r(tok, " \t\r\n", &tok);
    W = atoi(w);
    H = atoi(h);
}


void parse_row(char *line, int row) {
    char *num;
    char *verse = line;
    for (int col = 0; col < W; col++) {
        num = strtok_r(verse, " \t\r\n", &verse);
//        TEST
        image[row][col] = atoi(num);
    }

}
void image_to_array(char *file_name) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        error("Can't open file");
    }

    char buffer[BUFFER_SIZE + 1];

    fgets(buffer, BUFFER_SIZE, file);

    do {
        fgets(buffer, BUFFER_SIZE, file);
    } while (buffer[0] == '#');

    get_size(buffer);

    fgets(buffer, BUFFER_SIZE, file);


    image = calloc(H, sizeof(int *));
    result_image = calloc(H, sizeof(int *));
    for (int i = 0; i < H; i++) {
        image[i] = calloc(W, sizeof(int));
        result_image[i] = calloc(W, sizeof(int));
    }


    char *line = NULL;
    size_t len = 0;
    for (int i = 0; i < H; i++) {
        getline(&line, &len, file);
        parse_row(line, i);

    }

    fclose(file);

}

void array_to_image(char *file_name) {
    FILE *file = fopen(file_name, "w");
    if(file == NULL) {
        error("Can't open result file");
    }
    fprintf(file, "P2\n%d %d\n255\n", W, H);
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            fprintf(file,"%d ", result_image[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);

}


void * blocks(int index) {
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int size = W / threads;
    for (int i = index * size; i < (index + 1) * size; i++) {
        for (int j = 0; j < H; j++) {
            result_image[i][j] = 255 - image[i][j];
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    return (void *)time;
}

void * numbers(int index) {
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int size = 255 / threads;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (image[i][j] / size == index) {
                result_image[i][j] = 255 - image[i][j];
            }
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    return (void *)time;
}

void *image_to_negative(struct thread_arg *arg) {
    if(strcmp(arg->mode, "number")) {
        return numbers(arg->thread_n);
    }
    else {
        return blocks(arg->thread_n);
    }
}




int main(int argc, char *argv[]) {

    threads = atoi(argv[1]);
    char *mode = argv[2];
    char *input_file = argv[3];
    char *output_file = argv[4];
    image_to_array(input_file);
    FILE *results = fopen("Times.txt", "a");
    if(results == NULL) {
        error("Can't open times");
    }
    fprintf(results, "Mode: %s | threads: %d\n", mode, threads);

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    pthread_t *thread_id = calloc(threads, sizeof(pthread_t));
    struct thread_arg *args = calloc(threads, sizeof(struct thread_arg));
    for (int i = 0; i < threads; i++) {
        struct thread_arg t_arg;
        t_arg.mode = mode;
        t_arg.thread_n = i;
        args[i] = t_arg;
        pthread_create(&thread_id[i], NULL, (void*)image_to_negative, &args[i]);
    }

    for (int i = 0; i < threads; i++) {
        double *y;
        pthread_join(thread_id[i], (void *)&y);
        printf("Thread %d, time: %lf microseconds\n", i, *y);
        fprintf(results, "Thread %d, time: %lf microseconds\n", i, *y);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    printf("\nFULL TIME: %f\n", time);
    fprintf(results, "FULL TIME: %f\n\n", time);

    array_to_image(output_file);
    for (int i = 0; i < H; i++) {
        free(image[i]);
    }
    free(image);

    for (int i = 0; i < H; i++) {
        free(result_image[i]);
    }
    free(result_image);

    fclose(results);
    return 0;
}


