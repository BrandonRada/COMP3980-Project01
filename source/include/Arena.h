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
    int window_old_x;
    int window_old_y;
    int window_new_x;
    int window_new_y;
    bool window_changed;
};

void draw(struct arena *local_arena);

#endif    // MAIN_ARENA_H
