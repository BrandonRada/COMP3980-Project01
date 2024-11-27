//
// Created by jonathan on 11/21/24.
//

#ifndef MAIN_HANDLEINPUT_H
#define MAIN_HANDLEINPUT_H

// #include "../src/main.c"
#include "./Player.h"
#include <SDL2/SDL.h>
#define MOVEMENT_THRESHOLD 8000

void handle_input(SDL_GameController *controller, SDL_Event *event, struct player *my_player, int min_x, int min_y, int max_x, int max_y);

#endif    // MAIN_HANDLEINPUT_H
