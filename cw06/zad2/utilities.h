#ifndef CW06_UTILITIES_H
#define CW06_UTILITIES_H

#define _XOPEN_SOURCE 500
#include <sys/ipc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <time.h>
#include <mqueue.h>

#define MAX_LENGTH 256
#define SERVER_NAME "/serverQueue"
#define CLIENT_NAME_LENGTH 8
#define MAX_MSG_IN_QUEUE 5


struct message_t {
    long mtype;
    int pid;
    int id;
    int connect_id;
    char msg[MAX_LENGTH];
};


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

enum bool {
    TRUE = 1,
    FALSE = 0
};

#define MESSAGE_SIZE sizeof(struct message_t)

#define MSG_TYP (-100)

void error(char *msg);

void parse_command(char *statement, char *command, char *message);

void random_client_name(char *buffer);

void delete_queue(int queue);

int create_queue(char *name);

#endif //CW06_UTILITIES_H
