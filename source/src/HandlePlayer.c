#include "../include/HandlePlayer.h"

void handle_remote_player(struct player *remote_player, struct arena *local_arena, struct network_socket *net_socket)
{
    const char *token_x = NULL;
    const char *token_y = NULL;
    char       *saveptr = NULL;

    // Receive the remote player's position
    net_socket->valid_msg = read_socket(net_socket->sock, net_socket->buffer, &net_socket->peer_addr, net_socket->peer_addr_str);

    if(net_socket->valid_msg == 0)
    {
        // Extract x and y values from the buffer using strtok_r
        token_x = strtok_r(net_socket->buffer, ":", &saveptr);
        token_y = strtok_r(NULL, ":", &saveptr);
        // update remote_player's position
        if(token_x != NULL && token_y != NULL)
        {
            remote_player->x = (int)strtol(token_x, NULL, TEN);
            remote_player->y = (int)strtol(token_y, NULL, TEN);
        }
        net_socket->reconnect_attempts = 0;    // Reset reconnect attempts on successful communication
    }
    else if(net_socket->timer % FPS == 0)
    {
        (net_socket->reconnect_attempts)++;
        log_msg("Reconnect attempt: %d", net_socket->reconnect_attempts);
        if(strcmp(net_socket->peer_addr_str, DEFAULT_PEER_ADDR) != 0 && net_socket->reconnect_attempts == 2)
        {
            close(net_socket->sock);
            net_socket->sock = create_socket();
            bind_socket(net_socket->sock, &net_socket->my_addr);
            configure_peer_addr(&net_socket->peer_addr, net_socket->peer_addr_str);
        }
        if(strcmp(net_socket->peer_addr_str, DEFAULT_PEER_ADDR) == 0 || net_socket->reconnect_attempts > 4)
        {
            if(remote_player->x < 0 || remote_player->y < 0)
            {
                remote_player->x = (double)local_arena->max_x / 3;
                remote_player->y = (double)local_arena->max_y / 3;
            }

            // move remote player randomly
            remote_player->x = (int)(remote_player->x + (rand() % 3 - 1)) % local_arena->max_x;
            remote_player->y = (int)(remote_player->y + (rand() % 3 - 1)) % local_arena->max_y;
            if(remote_player->x < local_arena->min_x)
            {
                remote_player->x = local_arena->min_x;
            }
            else if(remote_player->x > local_arena->max_x)
            {
                remote_player->x = local_arena->max_x;
            }
            if(remote_player->y < local_arena->min_y)
            {
                remote_player->y = local_arena->min_y;
            }
            else if(remote_player->y > local_arena->max_y)
            {
                remote_player->y = local_arena->max_y;
            }
        }
        snprintf(net_socket->buffer, sizeof(net_socket->buffer), "%d:%d", local_arena->min_x, local_arena->min_y);
        write_socket(net_socket->sock, net_socket->buffer, &net_socket->peer_addr);
    }
}

void handle_local_player(SDL_GameController **controller, SDL_Event *event, struct player *local_player, const struct arena *local_arena)
{
    nodelay(stdscr, TRUE);
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
            log_msg("Controller Connected!");
        }
        else if(event->type == SDL_CONTROLLERDEVICEREMOVED && *controller)
        {
            SDL_GameControllerClose(*controller);
            *controller = NULL;
            log_msg("Controller Disconnected!");
        }
    }

    if(*controller)
    {
        handle_controller_input(*controller, local_player, local_arena);
        handle_keyboard_input(local_player, local_arena);
    }
    else
    {
        handle_keyboard_input(local_player, local_arena);
    }
    // Handle if the window changed and the player is now out of bounds
    handle_bounds(local_player, local_arena);

    // Update player's position
    mvprintw((int)local_player->y, (int)local_player->x, " ");
    local_player->x = local_player->temp_x;
    local_player->y = local_player->temp_y;
    mvprintw((int)local_player->y, (int)local_player->x, "%s", local_player->player_char);
}

void handle_controller_input(SDL_GameController *controller, struct player *local_player, const struct arena *local_arena)
{
    double distance = 0;
    int    angle    = 0;

    get_joystick_distance(controller, &distance);
    get_joystick_angle(controller, &angle, &distance);

    mvprintw(1, 1, "Joystick distance: %f\n", distance);
    mvprintw(2, 1, "Joystick angle: %d\n", angle);

    if(angle > TR && angle < TL)    // up
    {
        local_player->temp_y = local_player->y - SPEED_FACTOR;
        if(local_player->temp_y <= local_arena->min_y)
        {
            local_player->temp_y = local_player->y;
        }
    }
    else if(angle > BL && angle < BR)    // down
    {
        local_player->temp_y = local_player->y + SPEED_FACTOR;
        if(local_player->temp_y >= local_arena->max_y)
        {
            local_player->temp_y = local_player->y;
        }
    }

    if(angle > TL && angle < BL)    // left
    {
        local_player->temp_x = local_player->x - SPEED_FACTOR;
        if(local_player->temp_x <= local_arena->min_x)
        {
            local_player->temp_x = local_player->x;
        }
    }
    else if(!(angle > TR && angle < BR) && angle != -1)    // right
    {
        local_player->temp_x = local_player->x + SPEED_FACTOR;
        if(local_player->temp_x >= local_arena->max_x)
        {
            local_player->temp_x = local_player->x;
        }
    }
}

void handle_keyboard_input(struct player *local_player, const struct arena *local_arena)
{
    int ch;
    ch = getch();
    if(ch == KEY_UP)
    {
        local_player->temp_y = local_player->y - 1;
        if(local_player->temp_y <= local_arena->min_y)
        {
            local_player->temp_y = local_arena->min_y;
        }
    }
    else if(ch == KEY_DOWN)
    {
        local_player->temp_y = local_player->y + 1;
        if(local_player->temp_y >= local_arena->max_y)
        {
            local_player->temp_y = local_arena->max_y - 1;
        }
    }
    else if(ch == KEY_LEFT)
    {
        local_player->temp_x = local_player->x - 1;
        if(local_player->temp_x <= local_arena->min_x)
        {
            local_player->temp_x = local_arena->min_x;
        }
    }
    else if(ch == KEY_RIGHT)
    {
        local_player->temp_x = local_player->x + 1;
        if(local_player->temp_x >= local_arena->max_x)
        {
            local_player->temp_x = local_arena->max_x - 1;
        }
    }
}

// Gets the joysticks distance from idle
void get_joystick_distance(SDL_GameController *controller, double *distance)
{
    int x = abs(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX));
    int y = abs(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY));

    double norm_x = ((double)x / (double)CONTROLLER_JOYSTICK_MAX);
    double norm_y = ((double)y / (double)CONTROLLER_JOYSTICK_MAX);

    *distance = (norm_x > norm_y) ? norm_x : norm_y;
}

// Gets the joystick angle
void get_joystick_angle(SDL_GameController *controller, int *angle, const double *distance)
{
    int x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
    int y = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);

    double norm_x = ((double)x / (double)CONTROLLER_JOYSTICK_MAX);
    double norm_y = -((double)y / (double)CONTROLLER_JOYSTICK_MAX);

    double radians = atan2(norm_y, norm_x);
    *angle         = (int)(radians * (ANGLE_HALF / M_PI));

    if(*distance < THRESH)
    {
        *angle = -1;
    }
    else if(*angle < 0)
    {
        *angle += ANGLE_MAX;
    }
}

// This is used to check no matter if the move was made or not, check the updated temp vals, then mover the player back inside the border depending on where they are outside the border.
void handle_bounds(struct player *local_player, const struct arena *local_arena)
{
    // If to the left of left border
    if(local_player->temp_x < local_arena->min_x)
    {
        local_player->temp_x = local_arena->min_x + 1;
    }
    // If to the right of right border
    if(local_player->temp_x > local_arena->max_x)
    {
        local_player->temp_x = local_arena->max_x - 1;
    }
    // If above the top border
    if(local_player->temp_y < local_arena->min_y)
    {
        local_player->temp_y = local_arena->min_y + 1;
    }
    // If below bottom border
    if(local_player->temp_y > local_arena->max_y)
    {
        local_player->temp_y = local_arena->max_y - 1;
    }
}
