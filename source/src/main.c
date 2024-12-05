#include "../include/Arena.h"
#include "../include/HandleInput.h"
#include "../include/PearToPear.h"
#include "../include/Player.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#ifdef clang
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreserved-macro-identifier"
    #pragma clang diagnostic ignored "-Wreserved-identifier"
    #pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop
#ifdef clang
    #pragma clang diagnostic pop
#endif
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
#define DEFAULT_PEER_ADDR "255.255.255.255"

int main(int argc, char *argv[])
{
    struct player       local_player;
    struct player       remote_player;
    struct arena        local_arena;
    struct timespec     ts;
    SDL_GameController *controller = NULL;
    SDL_Event           event;

    int                opt;
    const char        *peer_addr_str = DEFAULT_PEER_ADDR;
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

    while((opt = getopt(argc, argv, "a:")) != -1)
    {
        if(opt == 'a')
        {
            peer_addr_str = optarg;
        }
    }

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
    configure_peer_addr(&peer_addr, peer_addr_str);

    mvprintw((int)local_player.y, (int)local_player.x, "%s", local_player.player_char);
    getmaxyx(stdscr, local_arena.window_old_y, local_arena.window_old_x);
    curs_set(0);
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
        valid_msg = receive_message(sock, buffer, &peer_addr, peer_addr_str);

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
            if(strcmp(peer_addr_str, DEFAULT_PEER_ADDR) != 0 && reconnect_attempts == 2)
            {
                close(sock);
                sock = create_socket();
                bind_socket(sock, &my_addr);
                configure_peer_addr(&peer_addr, peer_addr_str);
            }
            if(strcmp(peer_addr_str, DEFAULT_PEER_ADDR) == 0 || reconnect_attempts > 4)
            {
                if(remote_player.x < 0 || remote_player.y < 0)
                {
                    remote_player.x = (double)local_arena.max_x / 3;
                    remote_player.y = (double)local_arena.max_y / 3;
                }

                // move remote player randomly
                remote_player.x = (int)(remote_player.x + (rand() % 3 - 1)) % local_arena.max_x;
                remote_player.y = (int)(remote_player.y + (rand() % 3 - 1)) % local_arena.max_y;
                if(remote_player.x < local_arena.min_x)
                {
                    remote_player.x = local_arena.min_x;
                }
                else if(remote_player.x > local_arena.max_x)
                {
                    remote_player.x = local_arena.max_x;
                }
                if(remote_player.y < local_arena.min_y)
                {
                    remote_player.y = local_arena.min_y;
                }
                else if(remote_player.y > local_arena.max_y)
                {
                    remote_player.y = local_arena.max_y;
                }
            }
            snprintf(buffer, sizeof(buffer), "%d:%d", (int)local_player.x, (int)local_player.y);
            send_message(sock, buffer, &peer_addr);
        }

        mvprintw(local_arena.max_y - 2, 2, "Reconnect attempt: %d", reconnect_attempts);
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

    box(stdscr, 0, 0);
    refresh();
    if(local_arena->window_changed == true)
    {
        local_arena->window_changed = false;
        erase();
    }
}
