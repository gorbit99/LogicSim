#ifndef HOMEWORK_WINDOWING_H
#define HOMEWORK_WINDOWING_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "Utility.h"
#include <time.h>
#include <stdbool.h>
#include "Input.h"

typedef struct Window {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int id;
    int w;
    int h;
    bool shown;
    bool mouseFocus;
    bool keyboardFocus;
    bool requestClose;
	Input input;
} Window;

bool window_init_SDL();

void window_quit_SDL();

Window window_create(char *title, int w, int h, unsigned int windowFlags, unsigned int rendererFlags);

void window_show(Window *window);

void window_hide(Window *window);

void window_get_focus(Window *window);

void window_begin_event_handling(Window *window);

void window_handle_event(Window *window, SDL_Event *e);

void window_free(Window *window);

#endif //HOMEWORK_WINDOWING_H