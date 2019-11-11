#ifndef HOMEWORK_TEXTINPUT_H
#define HOMEWORK_TEXTINPUT_H

#include "SDL.h"
#include <string.h>
#include "Utility.h"
#include "ctype.h"
#include "SDL_ttf.h"
#include "Graphics.h"

typedef struct TextInput {
	char *text;
	size_t length;
	size_t cursorPosition;
	int cursorPosInPx;
	int lastUpdate;
	size_t selection;
	SDL_Surface *surface;
} TextInput;

TextInput textinput_create();

void textinput_start();

void textinput_handle_event(TextInput *textinput, SDL_Event *e);

void textinput_update_graphic(TextInput *textinput, TTF_Font *font);

SDL_Rect textinput_get_cursor_rect(TextInput *textinput, TTF_Font *font, int x, int y);

void textinput_render(TextInput *textinput, TTF_Font *font, int x, int y, SDL_Renderer *renderer);

void textinput_free(TextInput *textinput);

void textinput_end();

#endif //HOMEWORK_TEXTINPUT_H
