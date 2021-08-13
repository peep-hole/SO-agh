#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>


double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

void error( char * msg) {

    printf("%s\n", msg);
    exit(1);
}

const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

int search_file(char * file_name, char * pattern) {
    
    FILE * file1 = fopen(file_name, "r");

    int exit_flag = 0;

    while (!exit_flag) {

            char c[1000];
            int char_counter = 0;

            while(1) {


                if (fread(&c[char_counter], 1, 1, file1) != 1) {
                    exit_flag = 1;
                    break;
                }

                if(c[char_counter] == '\n') {
                    break;
                }

                if(c[char_counter] == ' ') {

                    break;
                }
                char_counter += 1;
            }

            char * tmp = (char *) calloc(char_counter, sizeof (char));

            for (int i = 0; i < char_counter; ++i) {

                tmp[i] = c[i];
            }

            if(strcmp(tmp, pattern) == 0) {

                return 1;
            }

            free(tmp);

        }

        return 0;
}

void search_dir(int max_depth, int actual_depth, char * actual_path, char * pattern) {

    if(actual_depth > max_depth) exit(0);
    
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(actual_path))) {

        char msg[5000];
        snprintf(msg, sizeof(msg), "Could not open directory %s", actual_path);

        error(msg);
    }

    while ( (entry = readdir(dir)) != NULL) {

        if (entry->d_type == DT_DIR) {

           char path[5000];

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            snprintf(path, sizeof(path), "%s/%s", actual_path, entry->d_name);

            if (fork() == 0) {
                search_dir(max_depth, actual_depth + 1, path, pattern);
                exit(0);
            } 
        }

        else {
            if(strcmp(get_filename_ext(entry->d_name), "txt") == 0) {

                char path[5000];
                snprintf(path, sizeof(path), "%s/%s", actual_path, entry->d_name);
                if(search_file(path, pattern)) { 
                    
                    printf("\nProcess %d found pattern in: %s\n", (int)getpid(), path);
                }
            }
        }

        pid_t wpid;
        int status = 0;

        while ((wpid = wait(&status)) > 0);
        
        
                
    }

}

int main (int argc, char **argv) {

    if (argc != 4)
        error("Wrong number og arguments");

    int max_depth = atoi(argv[3]);
    char * from = argv[1];
    char * pattern = argv[2];

    search_dir(max_depth, 0, from, pattern);


    return 0;
}