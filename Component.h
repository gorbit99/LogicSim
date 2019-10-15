#ifndef HOMEWORK_COMPONENT_H
#define HOMEWORK_COMPONENT_H

#include <SDL.h>
#include <stdio.h>

#include "Utility.h"
#include "Graphics.h"

typedef struct ComponentData {
	float x, y;
	int w, h;
	SDL_Texture *texture;
	size_t outCount;
	bool *out;
} ComponentData;

SDL_Surface *component_load_graphic(const char *path, float size, float thickness, TTF_Font *font);

ComponentData component_create_data(float x, float y, SDL_Texture *texture, size_t outCount);

void component_update_texture(ComponentData *dat, SDL_Texture *texture);

void component_free_data(ComponentData *dat);

void component_render(ComponentData *dat, SDL_Renderer *renderer, Point camPos);

#endif //HOMEWORK_COMPONENT_H
