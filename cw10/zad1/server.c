#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define MAX_PLAYERS 20
#define MAX_BACKLOG 10
#define MAX_MSG_LEN 256

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct
{
    char *name;
    int desc;
    int is_online;
} player;

player *players_data[MAX_PLAYERS] = {NULL};
int players_connected = 0;

int get_enemy(int id)
{
    if (id % 2 == 0)
        return id + 1;
    else
        return id - 1;
}

int add_player(char *name, int fd)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players_data[i] != NULL && strcmp(players_data[i]->name, name) == 0)
        {
            return -1;
        }
    }
    int id = -1;
    for (int i = 0; i < MAX_PLAYERS; i += 2)
    {
        if (players_data[i] != NULL && players_data[i + 1] == NULL)
        {
            id = i + 1;
            break;
        }
    }
    if (id == -1)
    {
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            if (players_data[i] == NULL)
            {
                id = i;
                break;
            }
        }
    }

    if (id != -1)
    {
        player *new_player = calloc(1, sizeof(player));
        new_player->name = calloc(MAX_MSG_LEN, sizeof(char));
        strcpy(new_player->name, name);
        new_player->desc = fd;
        new_player->is_online = 1;

        players_data[id] = new_player;
        players_connected++;
    }

    return id;
}

void error(char *msg)
{
    printf("ERROR: %s\n", msg);
    printf("ERRNO: %d\n", errno);
    exit(-1);
}

int name_to_id(char *name)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players_data[i] != NULL && strcmp(players_data[i]->name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}
void remove_player_data(int id)
{
    free(players_data[id]->name);
    free(players_data[id]);
    players_data[id] = NULL;
    players_connected--;
    int rival_id = get_enemy(id);
    if (players_data[rival_id] != NULL)
    {
        printf("Removing rival_id: %s\n", players_data[rival_id]->name);
        free(players_data[rival_id]->name);
        free(players_data[rival_id]);
        players_data[rival_id] = NULL;
        players_connected--;
    }
}
void remove_player(char *name)
{
    int index = -1;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players_data[i] != NULL && strcmp(players_data[i]->name, name) == 0)
        {
            index = i;
        }
    }
    printf("Removing player: %s\n", name);
    remove_player_data(index);
}
void check_for_players_inactivity()
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players_data[i] != NULL && !players_data[i]->is_online)
        {
            remove_player(players_data[i]->name);
        }
    }
}
void ping_players()
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players_data[i] != NULL)
        {
            send(players_data[i]->desc, "ping: ", MAX_MSG_LEN, 0);
            players_data[i]->is_online = 0;
        }
    }
}
void *ping()
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        check_for_players_inactivity();
        ping_players();
        pthread_mutex_unlock(&mutex);
        sleep(3);
    }
}

int create_local_socket(char *pathname)
{
    int local_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un local_sockaddr;
    memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
    local_sockaddr.sun_family = AF_UNIX;
    strcpy(local_sockaddr.sun_path, pathname);
    unlink(pathname);
    bind(local_socket, (struct sockaddr *)&local_sockaddr,
         sizeof(struct sockaddr_un));
    listen(local_socket, MAX_BACKLOG);
    return local_socket;
}

int create_net_socket(char *soc_port)
{
    struct addrinfo *info;
    struct addrinfo req;
    memset(&req, 0, sizeof(struct addrinfo));
    req.ai_family = AF_UNSPEC;
    req.ai_socktype = SOCK_STREAM;
    req.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, soc_port, &req, &info);
    int network_socket =
            socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    bind(network_socket, info->ai_addr, info->ai_addrlen);
    listen(network_socket, MAX_BACKLOG);
    freeaddrinfo(info);

    return network_socket;
}

int receiver(int local_socket, int network_socket)
{
    struct pollfd *fds = calloc(2 + players_connected, sizeof(struct pollfd));
    fds[0].fd = local_socket;
    fds[0].events = POLLIN;
    fds[1].fd = network_socket;
    fds[1].events = POLLIN;
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < players_connected; i++)
    {
        fds[i + 2].fd = players_data[i]->desc;
        fds[i + 2].events = POLLIN;
    }
    pthread_mutex_unlock(&mutex);
    poll(fds, players_connected + 2, -1);
    int result;
    // check for message
    for (int i = 0; i < players_connected + 2; i++)
    {
        if (fds[i].revents & POLLIN)
        {
            result = fds[i].fd;
            break;
        }
    }
    if (result == local_socket || result == network_socket)
    {
        result = accept(result, NULL, NULL);
    }
    free(fds);

    return result;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        error("./server [port] [path]");
    char *port = argv[1];
    char *socket_path = argv[2];
    srand(time(NULL));
    int local_socket = create_local_socket(socket_path);
    int net_socket = create_net_socket(port);

    pthread_t t;
    pthread_create(&t, NULL, ping, NULL);
    while (1)
    {
        int player_fd = receiver(local_socket, net_socket);
        char buffer[MAX_MSG_LEN + 1];
        recv(player_fd, buffer, MAX_MSG_LEN, 0);
        printf("%s\n", buffer);
        char *command = strtok(buffer, ":");
        char *arg = strtok(NULL, ":");
        char *player_name = strtok(NULL, ":");
        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0)
        {
            int index = add_player(player_name, player_fd);

            if (index == -1)
            {
                send(player_fd, "add:name_taken", MAX_MSG_LEN, 0);
                close(player_fd);
            }
            else if (index % 2 == 0)
            {
                send(player_fd, "add:no_enemy", MAX_MSG_LEN, 0);
            }
            else
            {
                int random_num = rand() % 100;
                int first, second;
                if (random_num % 2 == 0)
                {
                    first = index;
                    second = get_enemy(index);
                }
                else
                {
                    second = index;
                    first = get_enemy(index);
                }

                send(players_data[first]->desc, "add:O",
                     MAX_MSG_LEN, 0);
                send(players_data[second]->desc, "add:X",
                     MAX_MSG_LEN, 0);
            }
        }
        if (strcmp(command, "move") == 0)
        {
            int move = atoi(arg);
            int player = name_to_id(player_name);

            sprintf(buffer, "move:%d", move);
            send(players_data[get_enemy(player)]->desc, buffer, MAX_MSG_LEN,
                 0);
        }
        if (strcmp(command, "quit") == 0)
        {
            remove_player(player_name);
        }
        if (strcmp(command, "ping") == 0)
        {
            int player = name_to_id(player_name);
            if (player != -1)
            {
                players_data[player]->is_online = 1;
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    return 0;
}