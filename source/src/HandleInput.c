#include "../include/HandleInput.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SDL_CONTROLLER_AXIS_MAX 32767
#define D_MAX 360
#define D_HALF 180
#define THRESH 0.25
#define TR 45
#define TL 135
#define BR 315
#define BL 225

void handle_input(SDL_GameController **controller, SDL_Event *event, struct player *local_player, const struct arena *local_arena)
{
    local_player->temp_x = local_player->x;
    local_player->temp_y = local_player->y;

    while(SDL_PollEvent(event))
    {
        if(event->type == SDL_QUIT)
        {
            if(*controller)
            {
                SDL_GameControllerClose(*controller);
            }
            SDL_Quit();
            endwin();
            exit(EXIT_SUCCESS);
        }

        if(event->type == SDL_CONTROLLERDEVICEADDED)
        {
            if(!*controller)
            {
                *controller = SDL_GameControllerOpen(event->cdevice.which);
                if(*controller)
                {
                    mvprintw(1, 1, "Controller connected.\n");
                }
            }
        }
        else if(event->type == SDL_CONTROLLERDEVICEREMOVED)
        {
            if(*controller)
            {
                SDL_GameControllerClose(*controller);
                *controller = NULL;
                mvprintw(1, 1, "Controller disconnected.\n");
            }
        }
    }
    if(*controller)
    {
        handle_controller_input(*controller, event, local_player, local_arena);
    }
    else
    {
        handle_keyboard_input(event, local_player, local_arena);
    }
    // Update player's position
    mvprintw(local_player->y, local_player->x, " ");
    local_player->x = local_player->temp_x;
    local_player->y = local_player->temp_y;
    mvprintw(local_player->y, local_player->x, "%s", local_player->player_char);
}

void handle_controller_input(SDL_GameController *controller, const SDL_Event *event, struct player *local_player, const struct arena *local_arena)
{
    double distance = 0;
    int    angle    = 0;
    if(event->type == SDL_QUIT)
    {
        SDL_GameControllerClose(controller);
        SDL_Quit();
        exit(EXIT_SUCCESS);
    }

    get_joystick_distance(controller, &distance);
    get_joystick_angle(controller, &angle, &distance);

    if(distance > THRESH)
    {
        if(angle > TR && angle < TL)    // up
        {
            local_player->temp_y = local_player->y - 1;
            if(local_player->temp_y <= local_arena->min_y)
            {
                local_player->temp_y = local_player->y;
            }
        }
        else if(angle > BL && angle < BR)    // down
        {
            local_player->temp_y = local_player->y + 1;
            if(local_player->temp_y >= local_arena->max_y)
            {
                local_player->temp_y = local_player->y;
            }
        }

        if(angle > TL && angle < BL)    // left
        {
            local_player->temp_x = local_player->x - 1;
            if(local_player->temp_x <= local_arena->min_x)
            {
                local_player->temp_x = local_player->x;
            }
        }
        else if(!(angle > TR && angle < BR))    // right
        {
            local_player->temp_x = local_player->x + 1;
            if(local_player->temp_x >= local_arena->max_x)
            {
                local_player->temp_x = local_player->x;
            }
        }
    }

    /*if(event->type == SDL_CONTROLLERAXISMOTION)
    {
        // Adjust position based on axis movement with a threshold
        if(event->caxis.value < -MOVEMENT_THRESHOLD && event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
        {
            local_player->temp_x = local_player->x - 1;
            if(local_player->temp_x <= local_arena->min_x)
            {
                local_player->temp_x = local_player->x;
            }
        }
        else if(event->caxis.value > MOVEMENT_THRESHOLD && event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
        {
            local_player->temp_x = local_player->x + 1;
            if(local_player->temp_x >= local_arena->max_x)
            {
                local_player->temp_x = local_player->x;
            }
        }

        if(event->caxis.value < -MOVEMENT_THRESHOLD && event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
        {
            local_player->temp_y = local_player->y - 1;
            if(local_player->temp_y <= local_arena->min_y)
            {
                local_player->temp_y = local_player->y;
            }
        }
        else if(event->caxis.value > MOVEMENT_THRESHOLD && event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
        {
            local_player->temp_y = local_player->y + 1;
            if(local_player->temp_y >= local_arena->max_y)
            {
                local_player->temp_y = local_player->y;
            }
        }
    }*/

    if(event->type == SDL_CONTROLLERBUTTONDOWN)
    {
        if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP)
        {
            local_player->temp_y = local_player->y - 1;
            if(local_player->temp_y <= local_arena->min_y)
            {
                local_player->temp_y = local_player->y;
            }
        }
        else if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
        {
            local_player->temp_y = local_player->y + 1;
            if(local_player->temp_y >= local_arena->max_y)
            {
                local_player->temp_y = local_player->y;
            }
        }
        if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
        {
            local_player->temp_x = local_player->x + 1;
            if(local_player->temp_x >= local_arena->max_x)
            {
                local_player->temp_x = local_player->x;
            }
        }
        else if(event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
        {
            local_player->temp_x = local_player->x - 1;
            if(local_player->temp_x <= local_arena->min_x)
            {
                local_player->temp_x = local_player->x;
            }
        }
    }
}

void handle_keyboard_input(const SDL_Event *event, struct player *local_player, const struct arena *local_arena)
{
    if(event->type == SDL_KEYDOWN)
    {
        if(event->key.keysym.sym == SDLK_w || event->key.keysym.sym == SDLK_UP)
        {
            local_player->temp_y = local_player->y - 1;
            if(local_player->temp_y < local_arena->min_y)
            {
                local_player->temp_y = local_arena->min_y;
            }
        }
        else if(event->key.keysym.sym == SDLK_s || event->key.keysym.sym == SDLK_DOWN)
        {
            local_player->temp_y = local_player->y + 1;
            if(local_player->temp_y > local_arena->max_y)
            {
                local_player->temp_y = local_arena->max_y;
            }
        }
        if(event->key.keysym.sym == SDLK_a || event->key.keysym.sym == SDLK_LEFT)
        {
            local_player->temp_x = local_player->x - 1;
            if(local_player->temp_x < local_arena->min_x)
            {
                local_player->temp_x = local_arena->min_x;
            }
        }
        else if(event->key.keysym.sym == SDLK_d || event->key.keysym.sym == SDLK_RIGHT)
        {
            local_player->temp_x = local_player->x + 1;
            if(local_player->temp_x > local_arena->max_x)
            {
                local_player->temp_x = local_arena->max_x;
            }
        }
    }
}

// Function to get the dead zone distance
void get_joystick_distance(SDL_GameController *controller, double *distance)
{
    int x = abs(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX));
    int y = abs(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY));

    double norm_x = ((double)x / (double)SDL_CONTROLLER_AXIS_MAX);
    double norm_y = ((double)y / (double)SDL_CONTROLLER_AXIS_MAX);

    if(norm_x > norm_y)
    {
        *distance = norm_x;
    }
    else
    {
        *distance = norm_y;
    }
}

// Function to get the degree and direction
void get_joystick_angle(SDL_GameController *controller, int *angle, const double *distance)
{
    int x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
    int y = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);

    double norm_x = ((double)x / (double)SDL_CONTROLLER_AXIS_MAX);
    double norm_y = -((double)y / (double)SDL_CONTROLLER_AXIS_MAX);

    // Calculate the angle in radians
    double radians = atan2(norm_y, norm_x);

    // Convert the angle to degrees
    *angle = (int)(radians * (D_HALF / M_PI));
    if(*distance < THRESH)
    {
        *angle = -1;
    }
    else if(*angle < 0)
    {
        *angle += D_MAX;    // Convert to a range of 0 to 360 degrees
    }
}
