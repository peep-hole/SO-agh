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

void random_client_name(char *buffer){

    srand(time(NULL));

    buffer[0] = '/';
    buffer[CLIENT_NAME_LENGTH - 1] = '\0';

    for (int i = 1; i < CLIENT_NAME_LENGTH - 1; ++i) {
        buffer[i] = 'a' + (rand() % 26);
    }
}


void delete_queue(int queue) {

    if (msgctl(queue, IPC_RMID, NULL) == -1) {
        error("Can't delete queue");
    }
}

int create_queue(char *name) {

    struct mq_attr atr;
    atr.mq_maxmsg = MAX_MSG_IN_QUEUE;
    atr.mq_msgsize = MESSAGE_SIZE   ;
    atr.mq_curmsgs = 0;
    atr.mq_flags = 0;

   return mq_open(name, O_RDONLY | O_CREAT | O_EXCL, 0666, &atr);
}


