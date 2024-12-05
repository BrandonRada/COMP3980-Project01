#ifndef MAIN_ARENA_H
#define MAIN_ARENA_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreserved-macro-identifier"
    #pragma clang diagnostic ignored "-Wreserved-identifier"
    #pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop
#ifdef __clang__
    #pragma clang diagnostic pop
#endif
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
