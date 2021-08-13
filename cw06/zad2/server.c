#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <mqueue.h>
#include "utilities.h"

#define MAX_CLIENTS 10

int server_queue;

int server_size = 0;

struct Client {
    char name[CLIENT_NAME_LENGTH];
    int fd;
    int id;
    int connect_id;
    int is_connected;
    int is_available;
};

struct Client clients[MAX_CLIENTS];

void cleaner() {
    mq_unlink(SERVER_NAME);
}

int first_empty_slot() {
    if (server_size >= MAX_CLIENTS) {
        return -1;
    }
    for(int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].is_connected == 0) {
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
    printf("SENDING TO %d...\n", clients[id].id);
    strcpy(message.msg, msg);


    if (mq_send(clients[id].fd, (char *) &message, MESSAGE_SIZE, 1) == -1) {
        error("Could not send to client\n");
    }
    if (type == INIT) printf("SENT REGISTER CONFIRMATION...\n");
    else {
        printf("SENT\n");
    }

}

long receive(int queue, struct message_t *msg) {

    return mq_receive(queue, (char *) msg, MESSAGE_SIZE, NULL);
}

void stop_server() {
    struct message_t msg;
    int clients_stopped = 0;
    for(int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].is_connected == 1) {
            send(STOP, i, getpid(), -1, "");
            clients_stopped++;
        }
    }
    for (int i = 0; i < clients_stopped; ++i) {
        receive(server_queue, &msg);
    }
    printf("\nSERVER STOPS\n");
}

void handler(int signal) {
    stop_server();
    exit(0);
}

void client_stopped(struct message_t *msg) {
    int connected_with = clients[msg->id].connect_id;
    if (connected_with != -1) {
        send(DISCONNECT, connected_with, getpid(), -1, "");
    }
    clients[msg->id].is_connected = -1;
    server_size --;
    printf("CLIENT %d STOPPED\n", msg->id);
}

void disconnect_request(struct message_t *msg) {
    printf("DISCONNECTING CLIENT %d...\n", msg->id);
    int connected_with = clients[msg->id].connect_id;
    if (connected_with != -1) {
        clients[msg->id].is_available = 1;
        clients[msg->id].connect_id = -1;
        send(DISCONNECT, connected_with, getpid(), -1, "");
    }
    printf("DISCONNECTED\n");
}

void list_request(struct message_t *msg) {
    send(MESSAGE, msg->id, getpid(), -1, "USERS AVAILABLE: ");
    for (int i = 0; i < server_size; ++i) {
        if((clients[i].is_available)&&(clients[i].is_connected)&&(i != msg->id)) {
            char mess[MAX_LENGTH];
            sprintf(mess, " %d ", i);
            send(MESSAGE, msg->id, getpid(), -1, mess);
        }
    }
    printf("\n");
}

void register_request(struct message_t *msg) {
    printf("REGISTERING NEW CLIENT OF PID: %d...\n", msg->pid);
    printf("CLIENT QUEUE NAME: %s\n", msg->msg);
    int id = first_empty_slot();
    if (id == -1) {
        printf("Can't connect another user\n");
        kill(msg->pid, SIGINT);
    }
    else {
        clients[id].fd = mq_open(msg->msg, O_WRONLY);
        if (clients[id].fd == -1) {
            error("Can't open clients queue");
        }
        printf("CLIENT QID: %d\n", clients[id].fd);
        clients[id].is_connected = 1;
        clients[id].is_available = 1;
        clients[id].connect_id = -1;
        strcpy(clients[id].name, msg->msg);
        clients[id].id = id;
        server_size ++;


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
    else if ((clients[msg->connect_id].is_available != 1) || (clients[msg->connect_id].is_connected != 1)) {
        printf("Can't connect with this user\n");
        send(MESSAGE, msg->id, getpid(), -1, "\nSERVER: Could not connect with client\n");
    }
    else {
        clients[msg->id].connect_id = msg->connect_id;
        clients[msg->id].is_available = 0;
        clients[msg->connect_id].connect_id = msg->id;
        clients[msg->connect_id].is_available = 0;
        send(CONNECT, msg->id, getpid(), -1, clients[msg->connect_id].name);
        send(CONNECT, msg->connect_id, getpid(), -1, clients[msg->id].name);
        printf("CONNECTED\n");
    }
}

int main(int argc, char **argv) {
    for(int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i].is_available = 0;
        clients[i].is_connected = 0;
        clients[i].connect_id = -1;
        clients[i].fd = -1;
        clients[i].id = i;
    }

    atexit(cleaner);

    signal(SIGINT, handler);

    server_queue = create_queue(SERVER_NAME);

    printf("SERVER QUEUE: %d\n", server_queue);

    if (server_queue == -1) {
        printf("ERRNO: %d\n", errno);
        error("Can't create server queue");
    }

    struct message_t msg;

    printf("SERVER IS RUNNING...\n");

    for (;;) {
        if (mq_receive(server_queue, (char *) &msg, MESSAGE_SIZE, NULL) != -1) {

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

        else {
            printf("ERRNO %d\n", errno);
            error("COULD NOT RECEIVE MESSAGE");
        }

    }

}