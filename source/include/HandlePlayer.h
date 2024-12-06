#ifndef MAIN_HANDLEPLAYER_H
#define MAIN_HANDLEPLAYER_H

#include "./Arena.h"
#include "./Player.h"
#include "./PearToPear.h"
#include "./Constants.h"
#include "./LogToConsole.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void handle_local_player(SDL_GameController **controller, SDL_Event *event, struct player *local_player, const struct arena *local_arena);
void handle_controller_input(SDL_GameController *controller, struct player *local_player, const struct arena *local_arena);
void handle_keyboard_input(struct player *local_player, const struct arena *local_arena);
void get_joystick_distance(SDL_GameController *controller, double *distance);
void get_joystick_angle(SDL_GameController *controller, int *angle, const double *distance);
void handle_bounds(struct player *local_player, const struct arena *local_arena);
void handle_remote_player(struct player *remote_player, const struct arena *local_arena, struct network_socket *net_socket);
#endif    // MAIN_HANDLEPLAYER_H
