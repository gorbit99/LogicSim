#ifndef HOMEWORK_WINDOW_H
#define HOMEWORK_WINDOW_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <time.h>
#include "Utility.h"

bool window_init(SDL_Window **window, SDL_Renderer **renderer, const char *title, int w, int h, int windowFlags, int rendererFlags);
void window_cleanup(SDL_Window *window, SDL_Renderer *renderer);

#endif //HOMEWORK_WINDOW_H