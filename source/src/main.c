#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreserved-macro-identifier"
    #pragma clang diagnostic ignored "-Wreserved-identifier"
    #pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

#include "../include/Arena.h"
#include "../include/Constants.h"
#include "../include/HandlePlayer.h"
#include "../include/LogToConsole.h"
#include <arpa/inet.h>
#include <curses.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int                   opt;
    struct player         local_player;
    struct player         remote_player;
    struct arena          local_arena;
    struct timespec       ts;
    struct network_socket net_socket = {DEFAULT_PEER_ADDR, 0, {0}, {0}, 0, 0, 0, ""};
    SDL_GameController   *controller = NULL;
    SDL_Event             event;

    // Open new terminal for logging
    open_console();
    initscr();
    refresh();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, local_arena.max_y, local_arena.max_x);

    while((opt = getopt(argc, argv, "a:")) != -1)
    {
        if(opt == 'a')
        {
            net_socket.peer_addr_str = optarg;
        }
    }

    local_arena.window_changed = false;

    ts.tv_sec  = FIXED_UPDATE / NANO;
    ts.tv_nsec = FIXED_UPDATE % NANO;

    local_player.player_char  = "+";
    local_player.x            = (double)local_arena.max_x / 2;
    local_player.y            = (double)local_arena.max_y / 2;
    remote_player.player_char = "O";
    remote_player.x           = -1;
    remote_player.y           = -1;
    net_socket.timer          = 0;
    net_socket.valid_msg      = 0;

    if(SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
    {
        log_msg("SDL_Init Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if(SDL_NumJoysticks() > 0)
    {
        controller = SDL_GameControllerOpen(0);
        if(!controller)
        {
            log_msg("Could not open game controller: %s\n", SDL_GetError());
            SDL_Quit();
            return EXIT_FAILURE;
        }
    }
    else
    {
        log_msg("No game controllers connected.\n");
    }

    net_socket.sock = create_socket();
    bind_socket(net_socket.sock, &net_socket.my_addr);
    configure_peer_addr(&net_socket.peer_addr, net_socket.peer_addr_str);

    mvprintw((int)local_player.y, (int)local_player.x, "%s", local_player.player_char);
    getmaxyx(stdscr, local_arena.window_old_y, local_arena.window_old_x);
    curs_set(0);

    while(1)
    {
        draw(&local_arena);
        net_socket.timer++;
        mvprintw(local_arena.max_y - 1, 2, "Timer: %ld", net_socket.timer / FPS);
        handle_local_player(&controller, &event, &local_player, &local_arena);

        if(net_socket.valid_msg == 0 || net_socket.timer % FPS == 0)
        {
            snprintf(net_socket.buffer, sizeof(net_socket.buffer), "%d:%d", (int)local_player.x, (int)local_player.y);
            write_socket(net_socket.sock, net_socket.buffer, &net_socket.peer_addr);
        }

        handle_remote_player(&remote_player, &local_arena, &net_socket);

        mvprintw(local_arena.max_y - 2, 2, "Reconnect attempt: %d", net_socket.reconnect_attempts);

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
