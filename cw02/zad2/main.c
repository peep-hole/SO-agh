#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>


double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

void error( char * msg) {

    printf("%s\n", msg);
    exit(1);
}


int main (int argc, char **argv) {

    if (argc != 4) {

        error("Wrong number of arguments, should be 4!");
    }

    FILE* results = fopen("pomiar_zad_2.txt", "a");

    struct tms * tms_time[2];
    clock_t real_time[2];

    tms_time[0] = calloc(1, sizeof(struct tms *));
    tms_time[1] = calloc(1, sizeof(struct tms *));

    if (strcmp(argv[1], "sys") == 0) {

        real_time[0] = times(tms_time[0]);

        char buffer[256];
        int fd = open(argv[3], O_RDONLY);

        int i;
        int exit_flag = 0;
        int print_flag;

        while(!exit_flag) {

            print_flag = 0;

            for(i = 0; i < 256; ++i) {

                if (read(fd, &buffer[i], 1) != 1) {

                    i --;
                    exit_flag = 1;
                    break;
                }

                if (buffer[i] == '\n') {
                    i --;
                    break;
                }

                if (buffer[i] == argv[2][0]) {
                    print_flag = 1;
                }
            }

            if(print_flag) {

                for(int j = 0; j <= i; ++j) {

                    printf("%c", buffer[j]);
                }
                printf("\n");
            }

        }

        close(fd);
        real_time[1] = times(tms_time[1]);

        fprintf(results, "Using system functions\n");
        fprintf(results, "Times: Real = %lf, User = %lf, System = %lf \n\n",
                calculate_time(real_time[0], real_time[1]),
                calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime),
                calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));
    }

    else if (strcmp(argv[1], "lib") == 0) {

        real_time[0] = times(tms_time[0]);

        char buffer[256];
        FILE * file = fopen(argv[3], "r");

        int i;
        int exit_flag = 0;
        int print_flag;

        while(!exit_flag) {

            print_flag = 0;

            for(i = 0; i < 256; ++i) {

                if (fread(&buffer[i], 1, 1, file) != 1) {
                    exit_flag = 1;
                    break;
                }

                if (buffer[i] == '\n') {
                    break;
                }

                if (buffer[i] == argv[2][0]) {
                    print_flag = 1;
                }
            }

            if(print_flag) {

                for(int j = 0; j < i; ++j) {

                    printf("%c", buffer[j]);
                }
                printf("\n");
            }

        }

        fclose(file);
        real_time[1] = times(tms_time[1]);

        fprintf(results, "Using library functions\n");
        fprintf(results, "Times: Real = %lf, User = %lf, System = %lf \n\n",
                calculate_time(real_time[0], real_time[1]),
                calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime),
                calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));

    }

    else {

        error("No such running parameter!");
    }

    fclose(results);

    return 0;
}