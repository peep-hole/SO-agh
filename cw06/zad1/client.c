#include "utilities.h"
#include <stdio.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include <sys/select.h>

int server_queue;
int pv_queue;
int client_queue = -1;
int id = -1;

void send(long type, int pid, int connect_id, char *msg, int receiver) {
    struct message_t message;

    message.mtype = type;
    message.pid = pid;
    message.id = id;
    message.connect_id = connect_id;
    strcpy(message.msg, msg);

    if ((receiver == SERVER)&&(type != INIT)) {

        if(msgsnd(server_queue, &message, MESSAGE_SIZE, IPC_NOWAIT) == -1) {
            error("Can't send to server");
        }
    }
    else if (receiver == SERVER) {

        if(msgsnd(server_queue, &message, MESSAGE_SIZE, IPC_NOWAIT) == -1) {
            error("Can't send to server");
        }
    }
    else if (receiver == CLIENT) {
        if(msgsnd(client_queue, &message, MESSAGE_SIZE, IPC_NOWAIT) == -1) {
            error("Can't send to client");
        }
    }
}

void stop_client() {
    printf("\nSTOPPING...\n");
    send(STOP, getpid(), -1, "", SERVER);
//    sleep(1);
    printf("STOPPED\n");
}

void handler(int signal) {
    stop_client();
    exit(0);
}

void cleaner() {
    delete_queue(pv_queue);
}

long receive(int queue, struct message_t *msg) {

    long res = -1;

    res = msgrcv(queue, msg, MESSAGE_SIZE, MSG_TYP, IPC_NOWAIT);

    return res;
}

void init() {

    server_queue = msgget(public_key(), 0);
    if (server_queue == -1) {
        error("Can't open server's queue");
    }

    key_t pv_key = private_key();

    pv_queue = msgget(pv_key, IPC_CREAT | IPC_EXCL | 0600);

    printf("\nKEY: %d\n", pv_key);
    printf("\nQID: %d\n", pv_queue);

    if (pv_queue == -1) {
        error("Can't open private queue");
    }

    char message[MAX_LENGTH];
    sprintf(message, "%d", pv_key);

    send(INIT, getpid(), -1, message, SERVER);

    struct message_t msg;

    if (msgrcv(pv_queue, &msg, MESSAGE_SIZE, MSG_TYP, 0) == -1) {
        error("Can't receive from server");
    }


    id = msg.id;
    printf("REGISTERED\n");

}

void stop() {
    raise(SIGINT);
}

void disconnect() {
//    send(DISCONNECT, getpid(), -1,"", CLIENT);
    client_queue = -1;
    send(DISCONNECT, getpid(), -1,"", SERVER);
}

void list() {
    printf("SENDING LIST REQUEST TO SERVER...\n");
    send(LIST, getpid(), -1,  "", SERVER);
}
void connect(char *message) {
    int connect_id = atoi(message);

    send(CONNECT, getpid(), connect_id, "", SERVER);
}

void send_message(char *message) {
    send(MESSAGE, getpid(), -1, message, CLIENT);
}

void check_to_send() {

    char statement[MAX_LENGTH];
    char command[MAX_LENGTH];
    char message[MAX_LENGTH];

    fgets(statement, MAX_LENGTH, stdin);
    parse_command(statement, command, message);

    if (strcmp(command, "STOP") == 0) {
        stop();
    }

    else if (strcmp(command, "DISCONNECT") == 0) {
        disconnect();
    }

    else if (strcmp(command, "LIST") == 0) {
        list();
    }

    else if (strcmp(command, "CONNECT") == 0) {
        connect(message);
    }

    else if (strcmp(command, "MESSAGE") == 0) {
        if (client_queue != -1) {
            send_message(message);
        }
        else {
            printf("Not connected to any client\n");
        }
    }
}

void check_to_receive() {

    struct message_t msg;


    if (receive(pv_queue, &msg) != -1) {


        if (msg.mtype == STOP) {
            raise(SIGINT);
        } else if (msg.mtype == DISCONNECT) {
            send(DISCONNECT, getpid(), -1, "", SERVER);
            pv_queue = -1;
            printf("DISCONNECTED\n");
        } else if (msg.mtype == CONNECT) {
            client_queue = msg.connect_id;
            if (client_queue == -1) {
                printf("Can't connect to client\n");
            }
            printf("CONNECTED\n");
        }

        else if (msg.mtype == MESSAGE) {
            printf("-> %s", msg.msg);
            printf("\n");
        }
    }
}


int main(int argc, char  **argv) {

    atexit(cleaner);
    signal(SIGINT, handler);
    init();

    for(;;) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        struct timeval tmv;
        tmv.tv_sec = 0;
        tmv.tv_usec = 0;
        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tmv)) {
            check_to_send();
        }
        check_to_receive();
    }

    return 0;
}