#ifndef HOMEWORK_WINDOWING_H
#define HOMEWORK_WINDOWING_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "Utility.h"
#include <time.h>
#include <stdbool.h>

typedef struct Window {
    SDL_Window *window;
    SDL_Renderer *renderer;
} Window;

bool window_init_SDL();

void window_quit_SDL();

Window window_create(char *title, int w, int h, int windowFlags, int rendererFlags);

void window_show(Window *window);

void window_hide(Window *window);

void window_free(Window *window);

#endif //HOMEWORK_WINDOWING_H