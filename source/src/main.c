#include "../include/HandleInput.h"
#include "../include/Player.h"
#include <SDL2/SDL.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

#define PLAYER_START_POS 10;
// #define MOVEMENT_THRESHOLD 8000

void draw(void);

int main(void)
{
    //    getch();

    struct player       my_player;
    SDL_GameController *controller = NULL;
    SDL_Event           event;
    //    int                 temp_x;
    //    int                 temp_y;
    int max_x;
    int max_y;
    int min_x;
    int min_y;

    initscr();
    refresh();

    my_player.player_char = "8";
    my_player.x = PLAYER_START_POS my_player.y = PLAYER_START_POS

        //        temp_x = my_player.x;
        //    temp_y     = my_player.y;

        getmaxyx(stdscr, max_y, max_x);

    max_x--;
    max_y--;
    min_x = 0;
    min_y = 0;

    if(SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if(SDL_NumJoysticks() > 0)
    {
        controller = SDL_GameControllerOpen(0);
        if(!controller)
        {
            printf("Could not open game controller: %s\n", SDL_GetError());
            SDL_Quit();
            return EXIT_FAILURE;
        }
    }
    else
    {
        printf("No game controllers connected.\n");
        SDL_Quit();
        return EXIT_FAILURE;
    }

    mvprintw(my_player.y, my_player.x, "%s", my_player.player_char);
    while(1)
    {
        draw();
        handle_input(controller, &event, &my_player, min_x, min_y, max_x, max_y);
    }
}

void draw(void)
{
    char wall;
    char roof;

    curs_set(0);
    wall = '|';
    roof = '-';

    box(stdscr, (chtype)wall, (chtype)roof);

    refresh();
    // This messes it up
    //    endwin();
}
