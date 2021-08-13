#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#define MAX_MSG_LEN 256

int server_socket;
int socket_binded;
int is_0_player;
char msg_buffer[MAX_MSG_LEN + 1];
char *client_name;

typedef enum
{
    EMPTY,
    O,
    X
} sign;

typedef struct
{
    int move;
    sign signs[9];

} game;

int move(game *game, int pos)
{
    if (pos < 0 || pos > 9 || game->signs[pos] != EMPTY)
        return 0;
    game->signs[pos] = game->move ? O : X;
    game->move = !game->move;
    return 1;
}

sign look_for_winner(game *game)
{
    sign col = EMPTY;
    for (int x = 0; x < 3; x++)
    {
        sign first = game->signs[x];
        sign second = game->signs[x + 3];
        sign third = game->signs[x + 6];
        if (first == second && first == third && first != EMPTY)
            col = first;
    }
    if (col != EMPTY)
        return col;

    sign row = EMPTY;
    for (int y = 0; y < 3; y++)
    {
        sign first = game->signs[3 * y];
        sign second = game->signs[3 * y + 1];
        sign third = game->signs[3 * y + 2];
        if (first == second && first == third && first != EMPTY)
            row = first;
    }
    if (row != EMPTY)
        return row;

    sign first_diag = EMPTY;

    sign diag1 = game->signs[0];
    sign diag2 = game->signs[4];
    sign diag3 = game->signs[8];
    if (diag1 == diag2 && diag1 == diag3 && diag1 != EMPTY)
        first_diag = diag1;
    if (first_diag != EMPTY)
        return first_diag;
    sign sec_diag = EMPTY;
    diag1 = game->signs[2];
    diag2 = game->signs[4];
    diag3 = game->signs[6];
    if (diag1 == diag2 && diag1 == diag3 && diag1 != EMPTY)
        sec_diag = diag1;
    return sec_diag;
}
game main_game;

typedef enum
{
    START_GAME,
    WAIT,
    MOVE_WAIT,
    RIVAL_WAIT,
    MOVE,
    QUIT
} game_state;

game_state state = START_GAME;
char *command, *argument;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

void quit()
{
    char msg[MAX_MSG_LEN + 1];
    sprintf(msg, "quit: :%s", client_name);
    send(server_socket, msg, MAX_MSG_LEN, 0);
    exit(0);
}

void check_game_state()
{
    int is_win = 0;
    sign winner = look_for_winner(&main_game);
    if (winner != EMPTY)
    {
        if ((is_0_player && winner == O) || (!is_0_player && winner == X))
        {
            printf("WIN!\n");
        }
        else
        {
            printf("LOST!\n");
        }

        is_win = 1;
    }

    int is_draw = 1;
    for (int i = 0; i < 9; i++)
    {
        if (main_game.signs[i] == EMPTY)
        {
            is_draw = 0;
            break;
        }
    }

    if (is_draw && !is_win)
    {
        printf("DRAW\n");
    }

    if (is_win || is_draw)
    {
        state = QUIT;
    }
}
void split_msg(char *reply)
{
    command = strtok(reply, ":");
    argument = strtok(NULL, ":");
}
game new_game()
{
    game game = {1,
                 {EMPTY}};
    return game;
}
void draw_game()
{
    char sign;
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            if (main_game.signs[y * 3 + x] == EMPTY)
            {
                sign = y * 3 + x + 1 + '0';
            }
            else if (main_game.signs[y * 3 + x] == O)
            {
                sign = 'O';
            }
            else
            {
                sign = 'X';
            }
            printf("  %c  ", sign);
        }
        printf("\n_________________________\n");
    }
}

void play_game()
{
    while (1)
    {
        if (state == START_GAME)
        {
            if (strcmp(argument, "name_taken") == 0)
            {
                printf("Name is already taken\n");
                exit(1);
            }
            else if (strcmp(argument, "no_enemy") == 0)
            {
                printf("Waiting for rival\n");
                state = WAIT;
            }
            else
            {

                main_game = new_game();
                is_0_player = argument[0] == 'O';
                state = is_0_player ? MOVE : MOVE_WAIT;
            }
        }
        else if (state == WAIT)
        {
            pthread_mutex_lock(&mutex);
            while (state != START_GAME && state != QUIT)
            {
                pthread_cond_wait(&condition, &mutex);
            }
            pthread_mutex_unlock(&mutex);

            main_game = new_game();
            is_0_player = argument[0] == 'O';
            state = is_0_player ? MOVE : MOVE_WAIT;
        }
        else if (state == MOVE_WAIT)
        {
            printf("Waiting for rivals move\n");

            pthread_mutex_lock(&mutex);
            while (state != RIVAL_WAIT && state != QUIT)
            {
                pthread_cond_wait(&condition, &mutex);
            }
            pthread_mutex_unlock(&mutex);
        }
        else if (state == RIVAL_WAIT)
        {
            int pos = atoi(argument);
            move(&main_game, pos);
            check_game_state();
            if (state != QUIT)
            {
                state = MOVE;
            }
        }
        else if (state == MOVE)
        {
            draw_game();

            int pos;
            do
            {
                printf("Next move (%c): ", is_0_player ? 'O' : 'X');
                scanf("%d", &pos);
                pos--;
            } while (!move(&main_game, pos));

            draw_game();

            char msg[MAX_MSG_LEN + 1];
            sprintf(msg, "move:%d:%s", pos, client_name);
            send(server_socket, msg, MAX_MSG_LEN, 0);

            check_game_state();
            if (state != QUIT)
            {
                state = MOVE_WAIT;
            }
        }
        else if (state == QUIT)
        {
            quit();
        }
    }
}
void connect_to_server(char *path, int is_local)
{

    struct sockaddr_un local_sockaddr;

    if (is_local)
    {
        server_socket = socket(AF_UNIX, SOCK_DGRAM, 0);

        memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
        local_sockaddr.sun_family = AF_UNIX;
        strcpy(local_sockaddr.sun_path, path);

        connect(server_socket, (struct sockaddr *)&local_sockaddr,
                sizeof(struct sockaddr_un));
        socket_binded = socket(AF_UNIX, SOCK_DGRAM, 0);
        struct sockaddr_un binded_sockaddr;
        memset(&binded_sockaddr, 0, sizeof(struct sockaddr_un));
        binded_sockaddr.sun_family = AF_UNIX;
        sprintf(binded_sockaddr.sun_path, "%d", getpid());
        bind(socket_binded, (struct sockaddr *)&binded_sockaddr,
             sizeof(struct sockaddr_un));
    }
    else
    {
        struct addrinfo *info;

        struct addrinfo req;
        memset(&req, 0, sizeof(struct addrinfo));
        req.ai_family = AF_INET;
        req.ai_socktype = SOCK_DGRAM;

        getaddrinfo("127.0.0.1", path, &req, &info);

        server_socket =
                socket(info->ai_family, info->ai_socktype, info->ai_protocol);

        connect(server_socket, info->ai_addr, info->ai_addrlen);

        freeaddrinfo(info);
    }
    char msg[MAX_MSG_LEN + 1];
    sprintf(msg, "add: :%s", client_name);
    if (is_local)
    {
        sendto(socket_binded, msg, MAX_MSG_LEN, 0, (struct sockaddr *)&local_sockaddr, sizeof(struct sockaddr_un));
    }
    else
    {
        send(server_socket, msg, MAX_MSG_LEN, 0);
    }
}
void receive(int is_local)
{
    int game_running = 0;
    while (1)
    {
        if (is_local)
        {
            recv(socket_binded, msg_buffer, MAX_MSG_LEN, 0);
        }
        else
        {
            recv(server_socket, msg_buffer, MAX_MSG_LEN, 0);
        }

        split_msg(msg_buffer);

        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0)
        {
            state = START_GAME;
            if (!game_running)
            {
                pthread_t t;
                pthread_create(&t, NULL, (void *(*)(void *))play_game, NULL);
                game_running = 1;
            }
        }
        else if (strcmp(command, "move") == 0)
        {
            state = RIVAL_WAIT;
        }
        else if (strcmp(command, "quit") == 0)
        {
            state = QUIT;
            exit(0);
        }
        else if (strcmp(command, "ping") == 0)
        {
            sprintf(msg_buffer, "pong: :%s", client_name);
            send(server_socket, msg_buffer, MAX_MSG_LEN, 0);
        }
        pthread_cond_signal(&condition);
        pthread_mutex_unlock(&mutex);
    }
}
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "./player [client_name] [type] [destination]");
        return 1;
    }

    client_name = argv[1];
    char *type = argv[2];
    char *destination = argv[3];

    signal(SIGINT, quit);
    int is_local = strcmp(type, "local") == 0;
    connect_to_server(destination, is_local);

    receive(is_local);
    return 0;
}