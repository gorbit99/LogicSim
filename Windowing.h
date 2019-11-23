#ifndef HOMEWORK_WINDOWING_H
#define HOMEWORK_WINDOWING_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "Utility.h"
#include <time.h>
#include <stdbool.h>
#include "Input.h"

typedef struct SDLWindow {
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
} SDLWindow;

bool window_init_SDL();

void window_quit_SDL();

SDLWindow window_create(char *title, int w, int h, unsigned int windowFlags, unsigned int rendererFlags);

void window_show(SDLWindow *window);

void window_hide(SDLWindow *window);

void window_get_focus(SDLWindow *window);

void window_begin_event_handling(SDLWindow *window);

void window_handle_event(SDLWindow *window, SDL_Event *e);

void window_free(SDLWindow *window);

#endif //HOMEWORK_WINDOWING_H