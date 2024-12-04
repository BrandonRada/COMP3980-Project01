#include "../include/HandleInput.h"
#include "../include/Arena.h"
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
    // int ch;
    nodelay(stdscr, TRUE);
    // ch                   = getch();
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

        if(event->type == SDL_CONTROLLERDEVICEADDED && !*controller)
        {
            *controller = SDL_GameControllerOpen(event->cdevice.which);
        }
        else if(event->type == SDL_CONTROLLERDEVICEREMOVED && *controller)
        {
            SDL_GameControllerClose(*controller);
            *controller = NULL;
        }
    }

    // TESTING
    // if(*controller)
    // {
    //     double joystick_distance = 0;
    //     nodelay(stdscr, TRUE);
    //     get_joystick_distance(*controller, &joystick_distance);
    //
    //     if(joystick_distance >= THRESH)
    //     {
    //         nodelay(stdscr, FALSE);
    //         handle_controller_input(*controller, event, local_player, local_arena);
    //     }
    //     else
    //     {
    //         nodelay(stdscr, TRUE);
    //         mvprintw(1, 1, "Controller connected, but no joystick input detected.\n");
    //         // Check for keyboard input fallback
    //         if(ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT)
    //         {
    //             handle_keyboard_input(local_player, local_arena);
    //         }
    //     }
    // }
    // else
    // {
    //     nodelay(stdscr, TRUE);
    //     mvprintw(1, 1, "Controller disconnected.\n");
    //     // Handle keyboard input if no controller is connected
    //     if(ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT)
    //     {
    //         handle_keyboard_input(local_player, local_arena);
    //     }
    // }

    ///
    if(*controller)
    {
        handle_controller_input(*controller, event, local_player, local_arena);
        handle_keyboard_input(local_player, local_arena);
    }
    else
    {
        handle_keyboard_input(local_player, local_arena);
    }

    // if(*controller)
    // {
    //     handle_controller_input(*controller, event, local_player, local_arena);
    // }
    // else
    // {
    //     mvprintw(1, 1, "Controller disconnected.\n");
    // }
    // if(ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT)
    // {
    //     // handle_keyboard_input(event, local_player, local_arena);
    //     handle_keyboard_input(local_player, local_arena);
    // }
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

    get_joystick_distance(controller, &distance);
    get_joystick_angle(controller, &angle, &distance);

    mvprintw(1, 1, "Controller connected.\n");
    mvprintw(2, 1, "Joystick distance: %f\n", distance);
    mvprintw(3, 1, "Joystick angle: %d\n", angle);

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
    else if(!(angle > TR && angle < BR) && angle != -1)    // right
    {
        local_player->temp_x = local_player->x + 1;
        if(local_player->temp_x >= local_arena->max_x)
        {
            local_player->temp_x = local_player->x;
        }
    }

    // DPad movement, currently broken
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

// void handle_keyboard_input(const SDL_Event *event, struct player *local_player, const struct arena *local_arena)
void handle_keyboard_input(struct player *local_player, const struct arena *local_arena)

{
    int ch;
    ch = getch();
    // if()
    // {

    // }
    if(ch == KEY_UP)
    {
        local_player->temp_y = local_player->y - 1;
        if(local_player->temp_y < local_arena->min_y)
        {
            local_player->temp_y = local_arena->min_y;
        }
    }
    else if(ch == KEY_DOWN)
    {
        local_player->temp_y = local_player->y + 1;
        if(local_player->temp_y > local_arena->max_y)
        {
            local_player->temp_y = local_arena->max_y;
        }
    }
    else if(ch == KEY_LEFT)
    {
        local_player->temp_x = local_player->x - 1;
        if(local_player->temp_x < local_arena->min_x)
        {
            local_player->temp_x = local_arena->min_x;
        }
    }
    else if(ch == KEY_RIGHT)
    {
        local_player->temp_x = local_player->x + 1;
        if(local_player->temp_x > local_arena->max_x)
        {
            local_player->temp_x = local_arena->max_x;
        }
    }
    // // Keyboard input is currently broken
    // if(event->key.keysym.sym == SDLK_w || event->key.keysym.sym == SDLK_UP)
    // {
    //     local_player->temp_y = local_player->y - 1;
    //     if(local_player->temp_y < local_arena->min_y)
    //     {
    //         local_player->temp_y = local_arena->min_y;
    //     }
    // }
    // else if(event->key.keysym.sym == SDLK_s || event->key.keysym.sym == SDLK_DOWN)
    // {
    //     local_player->temp_y = local_player->y + 1;
    //     if(local_player->temp_y > local_arena->max_y)
    //     {
    //         local_player->temp_y = local_arena->max_y;
    //     }
    // }
    // if(event->key.keysym.sym == SDLK_a || event->key.keysym.sym == SDLK_LEFT)
    // {
    //     local_player->temp_x = local_player->x - 1;
    //     if(local_player->temp_x < local_arena->min_x)
    //     {
    //         local_player->temp_x = local_arena->min_x;
    //     }
    // }
    // else if(event->key.keysym.sym == SDLK_d || event->key.keysym.sym == SDLK_RIGHT)
    // {
    //     local_player->temp_x = local_player->x + 1;
    //     if(local_player->temp_x > local_arena->max_x)
    //     {
    //         local_player->temp_x = local_arena->max_x;
    //     }
    // }
}

// Gets the joysticks distance from idle
void get_joystick_distance(SDL_GameController *controller, double *distance)
{
    int x = abs(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX));
    int y = abs(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY));

    double norm_x = ((double)x / (double)SDL_CONTROLLER_AXIS_MAX);
    double norm_y = ((double)y / (double)SDL_CONTROLLER_AXIS_MAX);

    *distance = (norm_x > norm_y) ? norm_x : norm_y;
}

// Gets the joystick angle
void get_joystick_angle(SDL_GameController *controller, int *angle, const double *distance)
{
    int x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
    int y = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);

    double norm_x = ((double)x / (double)SDL_CONTROLLER_AXIS_MAX);
    double norm_y = -((double)y / (double)SDL_CONTROLLER_AXIS_MAX);

    double radians = atan2(norm_y, norm_x);
    *angle         = (int)(radians * (D_HALF / M_PI));

    if(*distance < THRESH)
    {
        *angle = -1;
    }
    else if(*angle < 0)
    {
        *angle += D_MAX;
    }
}
