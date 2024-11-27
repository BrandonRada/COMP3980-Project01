// #include "../include/HandleInput.h"
#include <SDL2/SDL.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

#define PLAYER_START_POS 10;
#define MOVEMENT_THRESHOLD 8000

struct player
{
    const char *player_char;
    int         x;
    int         y;
};

void draw(void);
void handle_input(SDL_GameController *controller, SDL_Event *event, struct player *my_player, int min_x, int min_y, int max_x, int max_y);

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

void handle_input(SDL_GameController *controller, SDL_Event *event, struct player *my_player, int min_x, int min_y, int max_x, int max_y)
{
    int temp_x = my_player->x;
    int temp_y = my_player->y;

    while(SDL_PollEvent(event))
    {
        if(event->type == SDL_QUIT)
        {
            SDL_GameControllerClose(controller);
            SDL_Quit();
            exit(EXIT_SUCCESS);
        }

        if(event->type == SDL_CONTROLLERAXISMOTION)
        {
            // Adjust position based on axis movement with a threshold
            if(event->caxis.value < -MOVEMENT_THRESHOLD && event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
            {
                temp_x = my_player->x - 1;
                if(temp_x <= min_x)
                {
                    temp_x = my_player->x;
                }
            }
            else if(event->caxis.value > MOVEMENT_THRESHOLD && event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
            {
                temp_x = my_player->x + 1;
                if(temp_x >= max_x)
                {
                    temp_x = my_player->x;
                }
            }

            if(event->caxis.value < -MOVEMENT_THRESHOLD && event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
            {
                temp_y = my_player->y - 1;
                if(temp_y <= min_y)
                {
                    temp_y = my_player->y;
                }
            }
            else if(event->caxis.value > MOVEMENT_THRESHOLD && event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
            {
                temp_y = my_player->y + 1;
                if(temp_y >= max_y)
                {
                    temp_y = my_player->y;
                }
            }

            // Update player's position
            mvprintw(my_player->y, my_player->x, " ");
            my_player->x = temp_x;
            my_player->y = temp_y;
            mvprintw(my_player->y, my_player->x, "%s", my_player->player_char);
        }

        if(event->type == SDL_CONTROLLERBUTTONDOWN)
        {
            //    printf("Button event: button %d %s\n", event.cbutton.button, event.type == SDL_CONTROLLERBUTTONDOWN ? "pressed" : "released");
            //    mvprintw(1, 1, "Button event: button %d %s\n", event.cbutton.button, event.type == SDL_CONTROLLERBUTTONDOWN ? "pressed" : "released");

            if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP)
            {
                temp_y = my_player->y - 1;
                if(temp_y <= min_y)
                {
                    temp_y = my_player->y;
                }
            }
            else if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
            {
                temp_y = my_player->y + 1;
                if(temp_y >= max_y)
                {
                    temp_y = my_player->y;
                }
            }
            else if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
            {
                temp_x = my_player->x + 1;
                if(temp_x >= max_x)
                {
                    temp_x = my_player->x;
                }
            }
            else if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
            {
                temp_x = my_player->x - 1;
                if(temp_x <= min_x)
                {
                    temp_x = my_player->x;
                }
            }
            mvprintw(my_player->y, my_player->x, " ");
            my_player->x = temp_x;
            my_player->y = temp_y;
            mvprintw(my_player->y, my_player->x, "%s", my_player->player_char);
        }
    }
}
