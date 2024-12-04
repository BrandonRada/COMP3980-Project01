#include "../include/Arena.h"
#include "../include/HandleInput.h"
#include "../include/Player.h"
#include <SDL2/SDL.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NANO 1000000000
#define FIXED_UPDATE (NANO / 30)

int main(void)
{
    struct player       local_player;
    struct arena        local_arena;
    struct timespec     ts;
    SDL_GameController *controller = NULL;
    SDL_Event           event;

    // int  old_x;
    // int  old_y;
    // int  new_x;
    // int  new_y;
    // bool window_changed;

    initscr();
    refresh();
    keypad(stdscr, TRUE);

    local_arena.window_changed = false;
    // window_changed             = false;

    ts.tv_sec  = FIXED_UPDATE / NANO;
    ts.tv_nsec = FIXED_UPDATE % NANO;

    getmaxyx(stdscr, local_arena.max_y, local_arena.max_x);
    local_player.player_char = "+";
    local_player.x           = local_arena.max_x / 2;
    local_player.y           = local_arena.max_y / 2;

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

    mvprintw(local_player.y, local_player.x, "%s", local_player.player_char);
    getmaxyx(stdscr, local_arena.window_old_y, local_arena.window_old_x);
    // getmaxyx(stdscr, old_x, old_y);
    while(1)
    {
        // getmaxyx(stdscr, new_x, new_y);
        // if(new_x != old_x || new_y != old_y)
        // {
        //     window_changed = true;
        // }
        // getmaxyx(stdscr, old_x, old_y);
        // draw(&local_arena, &window_changed);

        draw(&local_arena);
        getmaxyx(stdscr, local_arena.window_new_y, local_arena.window_new_x);
        if(local_arena.window_new_x != local_arena.window_old_x || local_arena.window_new_y != local_arena.window_old_y)
        {
            local_arena.window_changed = true;
        }
        getmaxyx(stdscr, local_arena.window_old_x, local_arena.window_old_y);

        handle_input(&controller, &event, &local_player, &local_arena);

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
        clear();
    }
}
