#ifndef MAIN_ARENA_H
#define MAIN_ARENA_H

#include <SDL2/SDL.h>
#include <curses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

struct arena
{
    int max_x;
    int max_y;
    int min_x;
    int min_y;
};

void draw(struct arena *local_arena);

#endif    // MAIN_ARENA_H
