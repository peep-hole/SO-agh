# include "utilities.h"

void error(char *msg){
    printf("ERROR: %s\n", msg);
    exit(1);
}

void parse_command(char *statement, char *command, char *message) {

    char *stat = strtok(statement, "\n");
    char *cmd = strtok(stat, " ");
    char *msg = strtok(NULL, "");

    if (cmd == NULL) {
         command[0] = '\0';
     }
     else {
         strcpy(command, cmd);
     }

    if (msg == NULL) {
        char tmp[MAX_LENGTH] = "";

        strcpy(message, tmp);
    }
    else {
        strcpy(message, msg);
    }

}

key_t public_key() {
    key_t key;
    key = ftok(getenv("HOME"), PROJECT_ID);
    if(key == -1) {
        error("Can't create a public key!");
    }
    return key;
}

key_t private_key() {
    key_t key;
    key = ftok(getenv("HOME"), getpid());
    if(key == -1) {
        error("Can't create a public key!");
    }
    return key;
}

void delete_queue(int queue) {

    if (msgctl(queue, IPC_RMID, NULL) == -1) {
        error("Can't delete queue");
    }
}

