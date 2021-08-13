#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include "utilities.h"

#define MAX_CLIENTS 10

int server_queue;

int server_size = 0;

int clients_queues[MAX_CLIENTS][2];
int clients_connections[MAX_CLIENTS];

void cleaner() {
    delete_queue(server_queue);
}

int first_empty_slot() {
    if (server_size >= MAX_CLIENTS) {
        return -1;
    }
    for(int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients_queues[i][1] == -1) {
            return i;
        }
    }
    return -1;
}

void send(long type, int id, int pid, int connect_id, char *msg) {
    struct message_t message;

    message.mtype = type;
    message.pid = pid;
    message.id = id;
    message.connect_id = connect_id;
    printf("SENDING TO %d...\n", clients_queues[id][1]);
    strcpy(message.msg, msg);

    if (msgsnd(clients_queues[id][1], &message, MESSAGE_SIZE, IPC_NOWAIT) == -1) {
        printf("->%d<-", errno);
        error("Could not send to client\n");
    }
    if (type == INIT) printf("SENT REGISTER CONFIRMATION...\n");
    else {
        printf("SENT\n");
    }

}

long receive(int queue, struct message_t *msg, int mode) {

    return msgrcv(queue, msg, MESSAGE_SIZE, MSG_TYP, mode);
}

void stop_server() {
    struct message_t msg;
    int clients_stopped = 0;
    for(int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients_queues[i][1] != -1) {
            send(STOP, i, getpid(), -1, "");
            clients_stopped++;
        }
    }
    for (int i = 0; i < clients_stopped; ++i) {
        receive(server_queue, &msg, 0);
    }
    printf("\nSERVER STOPS\n");
}

void handler(int signal) {
    stop_server();
    exit(0);
}

void client_stopped(struct message_t *msg) {
    int connected_with = clients_connections[msg->id];
    if (connected_with != -1) {
        send(DISCONNECT, connected_with, getpid(), -1, "");
        clients_connections[msg->id] = -1;
        clients_connections[connected_with] = -1;
    }
    clients_queues[msg->id][1] = -1;
    clients_queues[msg->id][0] = -1;
    server_size --;
    printf("CLIENT %d STOPPED\n", msg->id);
}

void disconnect_request(struct message_t *msg) {
    printf("DISCONNECTING CLIENT %d...\n", msg->id);
    int connected_with = clients_connections[msg->id];
    if (connected_with != -1) {
        clients_connections[msg->id] = -1;
        clients_connections[connected_with] = -1;
        send(DISCONNECT, connected_with, getpid(), -1, "");
    }
    printf("DISCONNECTED\n");
}

void list_request(struct message_t *msg) {
    send(MESSAGE, msg->id, getpid(), -1, "USERS AVAILABLE: ");
    for (int i = 0; i < server_size; ++i) {
        if((clients_connections[i] == -1)&&(clients_queues[i][1] != -1)&&(i != msg->id)) {
            char mess[MAX_LENGTH];
            sprintf(mess, " %d ", i);
            send(MESSAGE, msg->id, getpid(), -1, mess);
        }
    }
    printf("\n");
}

void register_request(struct message_t *msg) {
    printf("REGISTERING NEW CLIENT OF PID: %d...\n", msg->pid);
    printf("CLIENT KEY: %s\n", msg->msg);
    key_t key;
    sscanf(msg->msg, "%d", &key);
    printf("CLIENT KEY: %d\n", key);
    int id = first_empty_slot();
    if (id == -1) {
        printf("Can't connect another user\n");
        kill(msg->pid, SIGINT);
    }
    else {
        clients_queues[id][1] = msgget(key, 0);
        printf("CLIENT QID: %d\n", clients_queues[id][1]);
        clients_queues[id][0] = msg->pid;
        clients_connections[id] = -1;
        server_size ++;
        if (clients_queues[id][1] == -1) {
            error("Can't open clients queue");
        }

        send(INIT, id, getpid(), -1, "");
        printf("REGISTERED AS %d\n", id);
    }
}

void connect_request(struct message_t *msg) {
    printf("CLIENT %d WANTS TO CONNECT WITH %d...\n", msg->id, msg->connect_id);
    if ((msg->connect_id < 0) || msg->connect_id >= MAX_CLIENTS) {
        printf("WRONG CLIENT ID\n");
        send(MESSAGE, msg->id, getpid(), -1, "\nSERVER: Could not connect with client\n");
    }
    else if (clients_connections[msg->connect_id] != -1) {
        printf("Can't connect with this user\n");
        send(MESSAGE, msg->id, getpid(), -1, "\nSERVER: Could not connect with client\n");
    }
    else {
        clients_connections[msg->id] = msg->connect_id;
        clients_connections[msg->connect_id] = msg->id;
        send(CONNECT, msg->id, getpid(), clients_queues[msg->connect_id][1], "");
        send(CONNECT, msg->connect_id, getpid(), clients_queues[msg->id][1], "");
        printf("CONNECTED\n");
    }
}

int main(int argc, char **argv) {
    for(int i = 0; i < MAX_CLIENTS; ++i) {
        clients_connections[i] = -1;
        clients_queues[i][1] = -1;
    }

    atexit(cleaner);

    signal(SIGINT, handler);

    server_queue = msgget(public_key(), IPC_CREAT | IPC_EXCL | 0600);

    if (server_queue == -1) {
        error("Can't create server queue");
    }

    struct message_t msg;

    printf("SERVER IS RUNNING...\n");

    for (;;) {
        if (receive(server_queue, &msg, IPC_NOWAIT) != -1) {

            printf("\n");

            if (msg.mtype == STOP) {
                client_stopped(&msg);
            }
            else if (msg.mtype == DISCONNECT) {
                disconnect_request(&msg);
            }
            else if (msg.mtype == LIST) {
                list_request(&msg);
            }
            else if (msg.mtype == INIT) {
                register_request(&msg);
            }
            else if (msg.mtype == CONNECT) {
                connect_request(&msg);
            }
            else {
                error("Wrong command type");
            }
        }

    }

}