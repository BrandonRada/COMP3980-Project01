#include "../include/Arena.h"
#include "../include/HandleInput.h"
#include "../include/PearToPear.h"
#include "../include/Player.h"
#include <SDL2/SDL.h>
#include <arpa/inet.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NANO 1000000000
#define FPS 60
#define FIXED_UPDATE (NANO / FPS)
#define BUFSIZE 1024
#define TEN 10

int main(void)
{
    struct player       local_player;
    struct player       remote_player;
    struct arena        local_arena;
    struct timespec     ts;
    SDL_GameController *controller = NULL;
    SDL_Event           event;

    int                sock;
    struct sockaddr_in peer_addr;
    struct sockaddr_in my_addr;
    char               buffer[BUFSIZE];
    long               timer;
    int                valid_msg;
    int                reconnect_attempts = 0;
    initscr();
    refresh();
    keypad(stdscr, TRUE);

    local_arena.window_changed = false;

    ts.tv_sec  = FIXED_UPDATE / NANO;
    ts.tv_nsec = FIXED_UPDATE % NANO;

    getmaxyx(stdscr, local_arena.max_y, local_arena.max_x);
    local_player.player_char  = "+";
    local_player.x            = (double)local_arena.max_x / 2;
    local_player.y            = (double)local_arena.max_y / 2;
    remote_player.player_char = "O";
    remote_player.x           = -1;
    remote_player.y           = -1;
    timer                     = 0;
    valid_msg                 = 0;
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

    mvprintw((int)local_player.y, (int)local_player.x, "%s", local_player.player_char);
    getmaxyx(stdscr, local_arena.window_old_y, local_arena.window_old_x);

    while(1)
    {
        const char *token_x = NULL;
        const char *token_y = NULL;
        char       *saveptr = NULL;

        draw(&local_arena);
        timer++;
        mvprintw(local_arena.max_y - 1, 2, "Timer: %ld", timer / FPS);
        handle_input(&controller, &event, &local_player, &local_arena);

        if(valid_msg == 0 || timer % FPS == 0)
        {
            snprintf(buffer, sizeof(buffer), "%d:%d", (int)local_player.x, (int)local_player.y);
            send_message(sock, buffer, &peer_addr);
        }

        // Receive the remote player's position
        valid_msg = receive_message(sock, buffer, &peer_addr);

        if(valid_msg == 0)
        {
            // Extract x and y values from the buffer using strtok_r
            token_x = strtok_r(buffer, ":", &saveptr);
            token_y = strtok_r(NULL, ":", &saveptr);
            // update remote_player's position
            if(token_x != NULL && token_y != NULL)
            {
                remote_player.x = (int)strtol(token_x, NULL, TEN);
                remote_player.y = (int)strtol(token_y, NULL, TEN);
            }
            reconnect_attempts = 0;    // Reset reconnect attempts on successful communication
        }
        else if(timer % FPS == 0)
        {
            reconnect_attempts++;
            mvprintw(local_arena.max_y - 2, 2, "Reconnect attempt: %d", reconnect_attempts);
            if(reconnect_attempts == 2)
            {
                close(sock);
                sock = create_socket();
                bind_socket(sock, &my_addr);
                configure_peer_addr(&peer_addr);
                snprintf(buffer, sizeof(buffer), "%d:%d", (int)local_player.x, (int)local_player.y);
                send_message(sock, buffer, &peer_addr);
            }
        }

        mvprintw((int)remote_player.y, (int)remote_player.x, "%s", remote_player.player_char);
        draw(&local_arena);

        nanosleep(&ts, NULL);
    }
}

void draw(struct arena *local_arena)
{
    getmaxyx(stdscr, local_arena->window_new_y, local_arena->window_new_x);
    if(local_arena->window_new_x != local_arena->window_old_x || local_arena->window_new_y != local_arena->window_old_y)
    {
        local_arena->window_changed = true;
    }
    getmaxyx(stdscr, local_arena->window_old_x, local_arena->window_old_y);

    getmaxyx(stdscr, local_arena->max_y, local_arena->max_x);
    local_arena->max_x--;
    local_arena->max_y--;
    local_arena->min_x = 1;
    local_arena->min_y = 1;
    curs_set(0);
    box(stdscr, 0, 0);
    refresh();
    if(local_arena->window_changed == true)
    {
        local_arena->window_changed = false;
        erase();
    }
}
