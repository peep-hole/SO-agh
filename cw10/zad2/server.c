#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#define MAX_PLAYERS 20
#define MAX_BACKLOG 10
#define MAX_MSG_LEN 256

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct
{
    char *player_name;
    int desc;
    int is_online;
    struct sockaddr socket_address;
} player;

player *players[MAX_PLAYERS] = {NULL};
int players_count = 0;

int get_opponent(int id)
{
    if (id % 2 == 0)
        return id + 1;
    else
        return id - 1;
}

int add_player(char *player_name, struct sockaddr address, int desc)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i] != NULL && strcmp(players[i]->player_name, player_name) == 0)
        {
            return -1;
        }
    }
    int id = -1;
    for (int i = 0; i < MAX_PLAYERS; i += 2)
    {
        if (players[i] != NULL && players[i + 1] == NULL)
        {
            id = i + 1;
            break;
        }
    }
    if (id == -1)
    {
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            if (players[i] == NULL)
            {
                id = i;
                break;
            }
        }
    }

    if (id != -1)
    {
        player *new_player = calloc(1, sizeof(player));
        new_player->player_name = calloc(MAX_MSG_LEN, sizeof(char));
        strcpy(new_player->player_name, player_name);
        new_player->desc = desc;
        new_player->is_online = 1;
        new_player->socket_address = address;
        players[id] = new_player;
        players_count++;
    }

    return id;
}

void error(char *mess)
{
    printf("ERROR: %s\n", mess);
    printf("ERRNO: %d\n", errno);
    exit(-1);
}

int name_to_id(char *player_name)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i] != NULL && strcmp(players[i]->player_name, player_name) == 0)
        {
            return i;
        }
    }
    return -1;
}
void free_players_mem(int index)
{
    free(players[index]->player_name);
    free(players[index]);
    players[index] = NULL;
    players_count--;
    int opp = get_opponent(index);
    if (players[opp] != NULL)
    {
        printf("Removing opp: %s\n", players[opp]->player_name);
        sendto(players[opp]->desc, "quit: ", MAX_MSG_LEN, 0,
               &players[opp]->socket_address, sizeof(struct addrinfo));
        free(players[opp]->player_name);
        free(players[opp]);
        players[opp] = NULL;
        players_count--;
    }
}
void remove_player(char *name)
{
    int index = -1;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i] != NULL && strcmp(players[i]->player_name, name) == 0)
        {
            index = i;
        }
    }
    printf("Removing player: %s\n", name);
    free_players_mem(index);
}
void check_for_inactivity()
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i] != NULL && !players[i]->is_online)
        {
            remove_player(players[i]->player_name);
        }
    }
}
void ping_players()
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i] != NULL)
        {
            sendto(players[i]->desc, "ping: ", MAX_MSG_LEN, 0, &players[i]->socket_address, sizeof(struct addrinfo));
            players[i]->is_online = 0;
        }
    }
}
void ping()
{
    while (1)
    {
        printf("*PINGING*\n");
        pthread_mutex_lock(&mutex);
        check_for_inactivity();
        ping_players();
        pthread_mutex_unlock(&mutex);
        sleep(3);
    }
}

int create_local_socket(char *path)
{
    int local_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    struct sockaddr_un local_sockaddr;
    memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
    local_sockaddr.sun_family = AF_UNIX;
    strcpy(local_sockaddr.sun_path, path);
    unlink(path);
    bind(local_socket, (struct sockaddr *)&local_sockaddr,
         sizeof(struct sockaddr_un));
    listen(local_socket, MAX_BACKLOG);
    return local_socket;
}

int create_net_socket(char *port)
{
    struct addrinfo *info;
    struct addrinfo req;
    memset(&req, 0, sizeof(struct addrinfo));
    req.ai_family = AF_INET;
    req.ai_socktype = SOCK_DGRAM;
    req.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, port, &req, &info);
    int net_soc =
            socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    bind(net_soc, info->ai_addr, info->ai_addrlen);
    listen(net_soc, MAX_BACKLOG);
    freeaddrinfo(info);

    return net_soc;
}

int receive(int local_socket, int network_socket)
{
    struct pollfd descs[2];
    descs[0].fd = local_socket;
    descs[0].events = POLLIN;
    descs[1].fd = network_socket;
    descs[1].events = POLLIN;
    poll(descs, 2, -1);
    for (int i = 0; i < 2; i++)
    {
        if (descs[i].revents & POLLIN)
        {
            return descs[i].fd;
        }
    }

    return -1;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        error("./server [soc_port] [path]");
    char *soc_port = argv[1];
    char *soc_path = argv[2];
    srand(time(NULL));
    int local_soc = create_local_socket(soc_path);
    int net_soc = create_net_socket(soc_port);

    pthread_t thread;
    pthread_create(&thread, NULL, (void *(*)(void *))ping, NULL);
    while (1)
    {
        int desc = receive(local_soc, net_soc);
        char msg[MAX_MSG_LEN + 1];
        struct sockaddr address_of;
        socklen_t len_of = sizeof(struct sockaddr);
        recvfrom(desc, msg, MAX_MSG_LEN, 0, &address_of, &len_of);

        printf("%s\n", msg);
        char *command = strtok(msg, ":");
        char *argument = strtok(NULL, ":");
        char *player_name = strtok(NULL, ":");
        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0)
        {
            int id = add_player(player_name, address_of, desc);

            if (id == -1)
            {
                sendto(desc, "add:name_taken", MAX_MSG_LEN, 0, (struct sockaddr *)&address_of, sizeof(struct addrinfo));
            }
            else if (id % 2 == 0)
            {
                sendto(desc, "add:no_enemy", MAX_MSG_LEN, 0, (struct sockaddr *)&address_of, sizeof(struct addrinfo));
            }
            else
            {
                int rand_n = rand() % 100;
                int n1, n2;
                if (rand_n % 2 == 0)
                {
                    n1 = id;
                    n2 = get_opponent(id);
                }
                else
                {
                    n2 = id;
                    n1 = get_opponent(id);
                }

                sendto(players[n1]->desc, "add:O",
                       MAX_MSG_LEN, 0, &players[n1]->socket_address, sizeof(struct addrinfo));
                sendto(players[n2]->desc, "add:X",
                       MAX_MSG_LEN, 0, &players[n2]->socket_address, sizeof(struct addrinfo));
            }
        }
        if (strcmp(command, "move") == 0)
        {
            int move_data = atoi(argument);
            int player_moveing_name = name_to_id(player_name);

            sprintf(msg, "move_data:%d", move_data);
            sendto(players[get_opponent(player_moveing_name)]->desc, msg, MAX_MSG_LEN,
                   0, &players[get_opponent(player_moveing_name)]->socket_address, sizeof(struct addrinfo));
        }
        if (strcmp(command, "quit") == 0)
        {
            remove_player(player_name);
        }
        if (strcmp(command, "pong") == 0)
        {
            int player = name_to_id(player_name);
            if (player != -1)
            {
                players[player]->is_online = 1;
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    return 0;
}