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

    FILE* results = fopen("pomiar_zad_5.txt", "a");

    struct tms * tms_time[2];
    clock_t real_time[2];

    tms_time[0] = calloc(1, sizeof(struct tms *));
    tms_time[1] = calloc(1, sizeof(struct tms *));

    if (strcmp(argv[1], "sys") == 0) {

        real_time[0] = times(tms_time[0]);


        printf("\n");
        int fd1 = open(argv[2], O_RDONLY);
        int fd2 = open(argv[3], O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);

        char c;
        int char_counter = 0;

        while(1) {

            if (read(fd1, &c, 1) != 1) {

                break;
            }

            if (c == '\n') {
                char_counter = 0;
            }

            if (char_counter == 50) {

                write(fd2, "\n", 1);
                char_counter = 0;
            }

            char_counter += 1;

            write(fd2, &c, 1);
        }


        close(fd1);
        close(fd2);

        real_time[1] = times(tms_time[1]);

        fprintf(results, "Using system functions\n");
        fprintf(results, "Times: Real = %lf, User = %lf, System = %lf \n\n",
                calculate_time(real_time[0], real_time[1]),
                calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime),
                calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));
    }

    else if (strcmp(argv[1], "lib") == 0) {

        real_time[0] = times(tms_time[0]);


        FILE * file1 = fopen(argv[2], "r");
        FILE * file2 = fopen(argv[3], "w+");

        char c;
        int char_counter = 0;


        while(1) {

            if (fread(&c, 1, 1, file1) != 1) {

                break;
            }

            if (c == '\n') {
                char_counter = 0;
            }

            if (char_counter == 50) {

                fwrite("\n", 1, 1, file2);
                char_counter = 0;
            }

            char_counter += 1;

            fwrite(&c, 1, 1, file2);
        }


        fclose(file1);
        fclose(file2);


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