#include "../include/HandleInput.h"
#include <ncurses.h>
#include <stdlib.h>

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

            switch(event->cbutton.button)
            {
                case SDL_CONTROLLER_BUTTON_DPAD_UP:
                    temp_y = my_player->y - 1;
                    if(temp_y <= min_y)
                    {
                        temp_y = my_player->y;
                    }
                    break;

                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                    temp_y = my_player->y + 1;
                    if(temp_y >= max_y)
                    {
                        temp_y = my_player->y;
                    }
                    break;

                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    temp_x = my_player->x + 1;
                    if(temp_x >= max_x)
                    {
                        temp_x = my_player->x;
                    }
                    break;

                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    temp_x = my_player->x - 1;
                    if(temp_x <= min_x)
                    {
                        temp_x = my_player->x;
                    }
                    break;

                default:
                    break;
            }

            mvprintw(my_player->y, my_player->x, " ");
            my_player->x = temp_x;
            my_player->y = temp_y;
            mvprintw(my_player->y, my_player->x, "%s", my_player->player_char);
        }

        if(event->type == SDL_KEYDOWN)
        {
            printf("Key pressed: %d\n", event->key.keysym.sym);
            switch(event->key.keysym.sym)
            {
                case SDLK_UP:
                    temp_y = my_player->y - 1;
                    if(temp_y <= min_y)
                    {
                        temp_y = my_player->y;
                    }
                    break;

                case SDLK_a:
                    temp_y = my_player->y + 1;
                    if(temp_y >= max_y)
                    {
                        temp_y = my_player->y;
                    }
                    break;

                case SDLK_RIGHT:
                    temp_x = my_player->x + 1;
                    if(temp_x >= max_x)
                    {
                        temp_x = my_player->x;
                    }
                    break;

                case SDLK_LEFT:
                    temp_x = my_player->x - 1;
                    if(temp_x <= min_x)
                    {
                        temp_x = my_player->x;
                    }
                    break;

                default:
                    break;
            }

            // Update player's position for keyboard input
            mvprintw(my_player->y, my_player->x, " ");
            my_player->x = temp_x;
            my_player->y = temp_y;
            mvprintw(my_player->y, my_player->x, "%s", my_player->player_char);
        }
    }
}
