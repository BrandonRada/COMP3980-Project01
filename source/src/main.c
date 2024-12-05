#include "../include/Arena.h"
#include "../include/HandleInput.h"
#include "../include/PearToPear.h"
#include "../include/Player.h"
#include <SDL2/SDL.h>
#include <arpa/inet.h>
#include <curses.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NANO 1000000000
#define FIXED_UPDATE (NANO / 30)
#define BUFSIZE 1024
#define TEN 10
#define FIVETY 50000000L

struct network_data
{
    int                sock;
    struct sockaddr_in peer_addr;
    struct sockaddr_in my_addr;
    struct player     *remote_player;
};

static void *networking_thread(void *arg)
{
    struct network_data *net_data = (struct network_data *)arg;
    char                 buffer[BUFSIZE];
    struct timespec      req = {0, FIVETY};

    while(1)
    {
        const char *token_x;
        const char *token_y;
        char       *saveptr1 = NULL;
        char       *saveptr2 = NULL;

        receive_message(net_data->sock, buffer, &net_data->my_addr);
        token_x = strtok_r(buffer, ":", &saveptr1);
        token_y = strtok_r(NULL, ":", &saveptr2);

        if(token_x && token_y)
        {
            long temp_x = strtol(token_x, NULL, TEN);
            long temp_y = strtol(token_y, NULL, TEN);
            if(temp_x <= INT_MAX && temp_x >= INT_MIN)
            {
                net_data->remote_player->x = (int)temp_x;
            }
            if(temp_y <= INT_MAX && temp_y >= INT_MIN)
            {
                net_data->remote_player->y = (int)temp_y;
            }
        }
        snprintf(buffer, sizeof(buffer), "%d:%d", net_data->remote_player->x, net_data->remote_player->y);
        send_message(net_data->sock, buffer, &net_data->peer_addr);
        nanosleep(&req, NULL);
    }
    return NULL;    // Ensure a return statement
}

int main(void)
{
    struct player local_player;
    // struct player       remote_player;
    struct arena        local_arena;
    struct timespec     ts;
    SDL_GameController *controller = NULL;
    SDL_Event           event;

    int                 sock;
    struct sockaddr_in  peer_addr;
    struct sockaddr_in  my_addr;
    pthread_t           net_thread;
    struct network_data net_data;

    initscr();
    refresh();
    keypad(stdscr, TRUE);

    local_arena.window_changed = false;

    ts.tv_sec  = FIXED_UPDATE / NANO;
    ts.tv_nsec = FIXED_UPDATE % NANO;

    getmaxyx(stdscr, local_arena.max_y, local_arena.max_x);
    local_player.player_char = "+";
    local_player.x           = local_arena.max_x / 2;
    local_player.y           = local_arena.max_y / 2;
    // remote_player.player_char = "O";
    // remote_player.x           = local_arena.max_x / 4;
    // remote_player.y           = local_arena.max_y / 4;

    if(SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
    {
        mvprintw(1, 1, "SDL_Init Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if(SDL_NumJoysticks() > 0)
    {
        controller = SDL_GameControllerOpen(0);
        if(!controller)
        {
            mvprintw(1, 1, "Could not open game controller: %s\n", SDL_GetError());
            SDL_Quit();
            return EXIT_FAILURE;
        }
    }
    else
    {
        mvprintw(1, 1, "No game controllers connected.\n");
    }

    sock = create_socket();
    bind_socket(sock, &my_addr);
    configure_peer_addr(&peer_addr);

    // Setup network data and create networking thread
    net_data.sock      = sock;
    net_data.peer_addr = peer_addr;
    net_data.my_addr   = my_addr;
    // net_data.remote_player = &remote_player;
    pthread_create(&net_thread, NULL, networking_thread, &net_data);

    mvprintw(local_player.y, local_player.x, "%s", local_player.player_char);
    // mvprintw(remote_player.y, remote_player.x, "%s", remote_player.player_char);
    getmaxyx(stdscr, local_arena.window_old_y, local_arena.window_old_x);

    while(1)
    {
        getmaxyx(stdscr, local_arena.window_new_y, local_arena.window_new_x);
        if(local_arena.window_new_x != local_arena.window_old_x || local_arena.window_new_y != local_arena.window_old_y)
        {
            local_arena.window_changed = true;
        }
        local_arena.window_old_x = local_arena.window_new_x;
        local_arena.window_old_y = local_arena.window_new_y;

        draw(&local_arena);
        handle_input(&controller, &event, &local_player, &local_arena);

        draw(&local_arena);
        // mvprintw(remote_player.y, remote_player.x, "%s", remote_player.player_char);

        nanosleep(&ts, NULL);
    }
}

void draw(struct arena *local_arena)
{
    getmaxyx(stdscr, local_arena->max_y, local_arena->max_x);
    local_arena->max_x--;
    local_arena->max_y--;
    local_arena->min_x = 0;
    local_arena->min_y = 0;
    curs_set(0);
    box(stdscr, 0, 0);
    refresh();
    if(local_arena->window_changed == true)
    {
        local_arena->window_changed = false;
        erase();
    }
}
