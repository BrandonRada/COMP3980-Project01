#ifndef MAIN_HANDLEINPUT_H
#define MAIN_HANDLEINPUT_H

#include "./Arena.h"
#include "./Player.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define MOVEMENT_THRESHOLD 0.25

void handle_input(SDL_GameController **controller, SDL_Event *event, struct player *local_player, const struct arena *local_arena);
void handle_controller_input(SDL_GameController *controller, struct player *local_player, const struct arena *local_arena);
void handle_keyboard_input(struct player *local_player, const struct arena *local_arena);
void get_joystick_distance(SDL_GameController *controller, double *distance);
void get_joystick_angle(SDL_GameController *controller, int *angle, const double *distance);
void handle_out_of_bounds(struct player *local_player, const struct arena *local_arena);
#endif    // MAIN_HANDLEINPUT_H
