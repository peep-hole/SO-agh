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

    if ((argc != 2) && (argc != 4))
        error("Wrong number of arguments passed");

    FILE * results = fopen("pomiar_zad_1.txt", "a");

    struct tms * tms_time[2];
    clock_t real_time[2];

    tms_time[0] = calloc(1, sizeof(struct tms *));
    tms_time[1] = calloc(1, sizeof(struct tms *));

    char file1[100];
    char file2[100];

    if (argc == 2) {

        printf("Please insert file names:\nFile no. 1: ");
        scanf("%s", file1);
        printf("File no. 2: ");
        scanf("%s", file2);

    }

    else {

        strcpy(file1, argv[2]);
        strcpy(file2, argv[3]);
    }

    if(strcmp(argv[1], "sys") == 0) {

        real_time[0] = times(tms_time[0]);

        char c;
        int fd1 = open(file1, O_RDONLY);
        int fd2 = open(file2, O_RDONLY);
        int flag1 = 1, flag2 = 1;

        while ((flag1) && (flag2)) {

            while(1) {

                if (read(fd1, &c, 1) != 1) {
                    flag1 = 0;
                    printf("\n");
                    break;
                }

                if (c == '\n') {
                    printf("\n");
                    break;
                }

                printf("%c", c);
            }

            while(1) {

                if (read(fd2, &c, 1) != 1) {
                    printf("\n");
                    flag2 = 0;
                    break;
                }

                if (c == '\n') {
                    printf("\n");
                    break;
                }

                printf("%c", c);
            }
        }

        if(flag1 == 0) {
            while(read(fd2, &c, 1) == 1) {

                printf("%c", c);
            }
        }

        if(flag2 == 0) {
            while(read(fd1, &c, 1) == 1) {

                printf("%c", c);
            }
        }

        printf("\n");


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

        FILE * f1 = fopen(file1, "r");
        if (f1 == NULL) {
            error("Cannot open first file!");
        }
        FILE * f2 = fopen(file2, "r");
        if (f2 == NULL) {
            error("Cannot open second file!");
        }

        char c[1];
        int flag1 = 1, flag2 = 1;

        while ((flag1) && (flag2)) {

            while(1) {

                if (fread(c, 1, 1, f1) != 1) {
                    flag1 = 0;
                    printf("\n");
                    break;
                }

                if (c[0] == '\n') {
                    printf("\n");
                    break;
                }

                printf("%c", c[0]);
            }

            while(1) {

                if (fread(c, 1, 1, f2) != 1) {
                    printf("\n");
                    flag2 = 0;
                    break;
                }

                if (c[0] == '\n') {
                    printf("\n");
                    break;
                }

                printf("%c", c[0]);
            }
        }

        if(flag1 == 0) {
            while(fread(c, 1, 1, f2) == 1) {

                printf("%c", c[0]);
            }
        }

        if(flag2 == 0) {
            while(fread(c, 1, 1, f1) == 1) {

                printf("%c", c[0]);
            }
        }

        printf("\n");

        fclose(f1);
        fclose(f2);

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