#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#define MAX_MSG_LEN 256

int server_socket;
int is_o_client;
char msg_buffer[MAX_MSG_LEN + 1];
char *client_name;
typedef enum
{
    EMPTY,
    O,
    X
} sign;

typedef enum
{
    GAME_START,
    WAIT,
    WAIT_MOVE,
    WAIT_RIVAL_MOVE,
    MOVE,
    QUIT
} game_state;

typedef struct
{
    int move;
    sign signs[9];

} game;

int move(game *game, int position)
{
    if (position < 0 || position > 9 || game->signs[position] != EMPTY)
        return 0;
    game->signs[position] = game->move ? O : X;
    game->move = !game->move;
    return 1;
}

sign check_for_winner(game *game)
{
    sign column = EMPTY;
    for (int x = 0; x < 3; x++)
    {
        sign col1 = game->signs[x];
        sign col2 = game->signs[x + 3];
        sign col3 = game->signs[x + 6];
        if (col1 == col2 && col1 == col3 && col1 != EMPTY)
            column = col1;
    }
    if (column != EMPTY)
        return column;

    sign row = EMPTY;
    for (int y = 0; y < 3; y++)
    {
        sign row1 = game->signs[3 * y];
        sign row2 = game->signs[3 * y + 1];
        sign row3 = game->signs[3 * y + 2];
        if (row1 == row2 && row1 == row3 && row1 != EMPTY)
            row = row1;
    }
    if (row != EMPTY)
        return row;

    sign l_r_diag = EMPTY;

    sign l_r_diag1 = game->signs[0];
    sign l_r_diag2 = game->signs[4];
    sign l_r_diag3 = game->signs[8];
    if (l_r_diag1 == l_r_diag2 && l_r_diag1 == l_r_diag3 && l_r_diag1 != EMPTY)
        l_r_diag = l_r_diag1;
    if (l_r_diag != EMPTY)
        return l_r_diag;
    sign r_l_diag = EMPTY;
    l_r_diag1 = game->signs[2];
    l_r_diag2 = game->signs[4];
    l_r_diag3 = game->signs[6];
    if (l_r_diag1 == l_r_diag2 && l_r_diag1 == l_r_diag3 && l_r_diag1 != EMPTY)
        r_l_diag = l_r_diag1;
    return r_l_diag;
}
game game_board;

game_state state = GAME_START;
char *command, *argument;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

void quit()
{
    char buffer[MAX_MSG_LEN + 1];
    sprintf(buffer, "quit: :%s", client_name);
    send(server_socket, buffer, MAX_MSG_LEN, 0);
    exit(0);
}

void check_game_state()
{
    int is_win = 0;
    sign winner = check_for_winner(&game_board);
    if (winner != EMPTY)
    {
        if ((is_o_client && winner == O) || (!is_o_client && winner == X))
        {
            printf("WIN!\n");
        }
        else
        {
            printf("LOST!\n");
        }

        is_win = 1;
    }

    int drawing = 1;
    for (int i = 0; i < 9; i++)
    {
        if (game_board.signs[i] == EMPTY)
        {
            drawing = 0;
            break;
        }
    }

    if (drawing && !is_win)
    {
        printf("DRAW\n");
    }

    if (is_win || drawing)
    {
        state = QUIT;
    }
}
void parse_msg(char *msg)
{
    command = strtok(msg, ":");
    argument = strtok(NULL, ":");
}
game new_game_board()
{
    game game_board = {1,{EMPTY}};
    return game_board;
}
void draw_game()
{
    char sign;
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            if (game_board.signs[y * 3 + x] == EMPTY)
            {
                sign = y * 3 + x + 1 + '0';
            }
            else if (game_board.signs[y * 3 + x] == O)
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

void *play_game()
{
    while (1)
    {
        if (state == GAME_START)
        {
            if (strcmp(argument, "name_taken") == 0)
            {
                printf("This client_name already exists\n");
                exit(1);
            }
            else if (strcmp(argument, "no_enemy") == 0)
            {
                printf("Waiting for enemy to play\n");
                state = WAIT;
            }
            else
            {

                game_board = new_game_board();
                is_o_client = argument[0] == 'O';
                state = is_o_client ? MOVE : WAIT_MOVE;
            }
        }
        else if (state == WAIT)
        {
            pthread_mutex_lock(&mutex);
            while (state != GAME_START && state != QUIT)
            {
                pthread_cond_wait(&condition, &mutex);
            }
            pthread_mutex_unlock(&mutex);

            game_board = new_game_board();
            is_o_client = argument[0] == 'O';
            state = is_o_client ? MOVE : WAIT_MOVE;
        }
        else if (state == WAIT_MOVE)
        {
            printf("Waiting for rivals move\n");

            pthread_mutex_lock(&mutex);
            while (state != WAIT_RIVAL_MOVE && state != QUIT)
            {
                pthread_cond_wait(&condition, &mutex);
            }
            pthread_mutex_unlock(&mutex);
        }
        else if (state == WAIT_RIVAL_MOVE)
        {
            int riv_pos = atoi(argument);
            move(&game_board, riv_pos);
            check_game_state();
            if (state != QUIT)
            {
                state = MOVE;
            }
        }
        else if (state == MOVE)
        {
            draw_game();

            int player_pos;
            do
            {
                printf("Next move (%c): ", is_o_client ? 'O' : 'X');
                scanf("%d", &player_pos);
                player_pos--;
            } while (!move(&game_board, player_pos));

            draw_game();

            char buffer[MAX_MSG_LEN + 1];
            sprintf(buffer, "move:%d:%s", player_pos, client_name);
            send(server_socket, buffer, MAX_MSG_LEN, 0);

            check_game_state();
            if (state != QUIT)
            {
                state = WAIT_MOVE;
            }
        }
        else if (state == QUIT)
        {
            quit();
        }
    }
}
void connect_with_server(char *connection_type, char *path_or_name)
{

    if (strcmp(connection_type, "local") == 0)
    {
        server_socket = socket(AF_UNIX, SOCK_STREAM, 0);

        struct sockaddr_un local_sockaddr;
        memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
        local_sockaddr.sun_family = AF_UNIX;
        strcpy(local_sockaddr.sun_path, path_or_name);

        connect(server_socket, (struct sockaddr *)&local_sockaddr,
                sizeof(struct sockaddr_un));
    }
    else
    {
        struct addrinfo *info;

        struct addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        getaddrinfo("localhost", path_or_name, &hints, &info);

        server_socket =
                socket(info->ai_family, info->ai_socktype, info->ai_protocol);

        connect(server_socket, info->ai_addr, info->ai_addrlen);

        freeaddrinfo(info);
    }
}
void receiver()
{
    int game_thread_running = 0;
    while (1)
    {
        recv(server_socket, msg_buffer, MAX_MSG_LEN, 0);
        parse_msg(msg_buffer);

        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0)
        {
            state = GAME_START;
            if (!game_thread_running)
            {
                pthread_t t;
                pthread_create(&t, NULL, play_game, NULL);
                game_thread_running = 1;
            }
        }
        else if (strcmp(command, "move") == 0)
        {
            state = WAIT_RIVAL_MOVE;
        }
        else if (strcmp(command, "quit") == 0)
        {
            state = QUIT;
            exit(0);
        }
        else if (strcmp(command, "ping") == 0)
        {
            sprintf(msg_buffer, "ping: :%s", client_name);
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
        fprintf(stderr, "./player [client_name] [connection_type] [path_or_name]");
        return 1;
    }

    client_name = argv[1];
    char *connection_type = argv[2];
    char *path_or_name = argv[3];

    signal(SIGINT, quit);
    connect_with_server(connection_type, path_or_name);
    char buffer[MAX_MSG_LEN + 1];
    sprintf(buffer, "add: :%s", client_name);
    send(server_socket, buffer, MAX_MSG_LEN, 0);

    receiver();
    return 0;
}