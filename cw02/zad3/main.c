#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>

void error( char * msg) {

    printf("%s\n", msg);
    exit(1);
}

int power(int base, int exponent) {

    if (exponent < 0) {

        error("Sorry, power function is build for positive int exponents.");
    }

    int result = 1;

    if (exponent == 0) return result;

    for(int i = 0; i < exponent; ++i) {

        result *= base;
    }

    return result;
}

int is_power(int number) {

    int i = 1;

    while (i*i <= number) {

        if(i*i == number) return 1;
        i++;
    }

    return 0;
}

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}


int main (int argc, char **argv) {

    if ((argc != 2))
        error("Wrong number of arguments passed");

    FILE * results = fopen("pomiar_zad_3.txt", "a");

    struct tms * tms_time[2];
    clock_t real_time[2];

    tms_time[0] = calloc(1, sizeof(struct tms *));
    tms_time[1] = calloc(1, sizeof(struct tms *));

    char buffer[9];

    if(strcmp(argv[1], "sys") == 0) {

        real_time[0] = times(tms_time[0]);

        int fd_dane = open("dane.txt", O_RDONLY);

        int fd_a = open("a.txt", O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
        int fd_b = open("b.txt", O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
        int fd_c = open("c.txt", O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);

        int number;
        int exit_flag = 0;

        int number_flag;

        int even_counter = 0;

        while(!exit_flag) {

            number = 0;
            number_flag = 0;

            int i;

            for(i = 0; i < 9; ++i) {

                if (read(fd_dane, &buffer[i], 1) != 1) {

                    i --;
                    exit_flag = 1;
                    break;
                }

                if (buffer[i] == '\n') {
                    i --;
                    break;
                }
            }

            for (int j = i; j >= 0; --j) {

                number += (int)(buffer[j] - '0') * power(10, i - j);
                number_flag = 1;
            }


            if (number_flag) {

                if(number % 2 == 0) even_counter++;

                if( (number > 9) && ( ( (number / 10) % 10 == 0) || ( (number / 10) % 10 == 7) ) ) {

                    for (int j = 0; j <= i; ++j) {

                        write(fd_b, &buffer[j], 1);
                    }
                    write(fd_b, "\n", 1);
                }

                if (is_power(number)) {

                    for (int j = 0; j <= i; ++j) {
                        write(fd_c, &buffer[j], 1);
                    }
                    write(fd_c, "\n", 1);
                }

            }
        }

        int count = 0;
        int tmp = even_counter;
        while (tmp != 0) {
            tmp /= 10;
            count ++;
        }
        char * tmp_buf = (char *)(calloc(count, sizeof(char)));

        sprintf(tmp_buf, "%d", even_counter);

        write(fd_a, "Liczb parzystych jest ", 22);
        write(fd_a, tmp_buf, count);
        write(fd_a, "\n", 1);

        close(fd_dane);
        close(fd_a);
        close(fd_b);
        close(fd_c);

        real_time[1] = times(tms_time[1]);

        fprintf(results, "Using system functions\n");
        fprintf(results, "Times: Real = %lf, User = %lf, System = %lf \n\n",
                calculate_time(real_time[0], real_time[1]),
                calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime),
                calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));
    }

    else if (strcmp(argv[1], "lib") == 0) {

        real_time[0] = times(tms_time[0]);

        FILE* file_dane = fopen("dane.txt", "r");

        FILE * file_a = fopen("a.txt", "w");
        FILE * file_b = fopen("b.txt", "w");
        FILE * file_c = fopen("c.txt", "w");

        int number;
        int exit_flag = 0;

        int number_flag;

        int even_counter = 0;

        while(!exit_flag) {

            number = 0;
            number_flag = 0;

            int i;

            for(i = 0; i < 9; ++i) {

                if (fread(&buffer[i], 1, 1, file_dane) != 1) {

                    i --;
                    exit_flag = 1;
                    break;
                }

                if (buffer[i] == '\n') {
                    i --;
                    break;
                }
            }

            for (int j = i; j >= 0; --j) {

                number += (int)(buffer[j] - '0') * power(10, i - j);
                number_flag = 1;
            }


            if (number_flag) {

                if(number % 2 == 0) even_counter++;

                if( (number > 9) && ( ( (number / 10) % 10 == 0) || ( (number / 10) % 10 == 7) ) ) {

                    fwrite(buffer, 1, i + 1, file_b);

                    fwrite("\n", 1, 1, file_b);
                }

                if (is_power(number)) {

                    fwrite(buffer, 1, i + 1, file_c);

                    fwrite("\n", 1, 1, file_c);
                }

            }
        }

        int count = 0;
        int tmp = even_counter;
        while (tmp != 0) {
            tmp /= 10;
            count ++;
        }
        char * tmp_buf = (char *)(calloc(count, sizeof(char)));

        sprintf(tmp_buf, "%d", even_counter);

        fwrite("Liczb parzystych jest ", 1, 22, file_a);
        fwrite(tmp_buf, 1, count, file_a);
        fwrite("\n", 1, 1, file_a);

        fclose(file_dane);
        fclose(file_a);
        fclose(file_b);
        fclose(file_c);

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
