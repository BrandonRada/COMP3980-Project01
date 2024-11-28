#include "../include/Arena.h"
#include "../include/HandleInput.h"
#include "../include/Player.h"
#include <SDL2/SDL.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

#define C 10000

int main(void)
{
    struct player       local_player;
    struct arena        local_arena;
    SDL_GameController *controller = NULL;
    SDL_Event           event;
    int                 count = 0;

    initscr();
    refresh();
    keypad(stdscr, TRUE);

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
    while(1)
    {
        double distance = 0;
        int    angle    = 0;

        draw(&local_arena);

        get_joystick_distance(controller, &distance);
        get_joystick_angle(controller, &angle, &distance);

        mvprintw(2, 1, "Joystick distance: %f\n", distance);
        mvprintw(3, 1, "Joystick angle: %d\n", angle);
        if(count > C)
        {
            handle_input(&controller, &event, &local_player, &local_arena);
            count = 0;
        }

        count++;
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
}
