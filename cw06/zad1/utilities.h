#ifndef CW06_UTILITIES_H
#define CW06_UTILITIES_H

#define _XOPEN_SOURCE 500
#include <sys/ipc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>

#define MAX_LENGTH 1024
#define PROJECT_ID 'a'


enum types {
    STOP = 1,
    DISCONNECT = 2,
    LIST = 3,
    INIT = 4,
    CONNECT = 5,
    MESSAGE = 6
};

enum receiver {
    SERVER = 1,
    CLIENT = 2
};

struct message_t {
    long mtype;
    int pid;
    int id;
    int connect_id;
    char msg[MAX_LENGTH];
};

enum bool {
    TRUE = 1,
    FALSE = 0
};

#define MESSAGE_SIZE sizeof(struct message_t) - sizeof(long)
#define MSG_TYP (-100)

void error(char *msg);

void parse_command(char *statement, char *command, char *message);

key_t public_key();

key_t private_key();

void delete_queue(int queue);

#endif //CW06_UTILITIES_H
