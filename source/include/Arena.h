#ifndef MAIN_ARENA_H
#define MAIN_ARENA_H

#include <SDL2/SDL.h>
#include <curses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

struct arena
{
    /*__attribute__((unused)) */ int max_x;
    /*__attribute__((unused)) */ int max_y;
    /*__attribute__((unused)) */ int min_x;
    /*__attribute__((unused)) */ int min_y;
};

void draw(struct arena *local_arena);

#endif    // MAIN_ARENA_H
