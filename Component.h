#ifndef HOMEWORK_COMPONENT_H
#define HOMEWORK_COMPONENT_H

#include <SDL.h>
#include <stdio.h>
#include <math.h>

#include "Utility.h"
#include "Graphics.h"

typedef enum PinType {
	PIN_IN,
	PIN_OUT
} PinType;

typedef struct Pin {
	char name[17];
	PinType type;
	float angle;
	Point pos;
} Pin;

typedef struct PinData {
	Pin *pins;
	int pinCount;
} PinData;

typedef struct ComponentData {
	float x, y;
	int w, h;
	SDL_Texture *texture;
	PinData pinData;
} ComponentData;

SDL_Surface *component_load_graphic(const char *path, float size, float thickness, TTF_Font *font);
PinData component_load_pin_data(const char *path, float size);

ComponentData component_create(float x, float y, char *name, float size, float thickness,
		TTF_Font *font, SDL_Renderer *renderer);

void component_update_texture(ComponentData *dat, SDL_Texture *texture);

void component_free_data(ComponentData *dat);

void component_render(ComponentData *dat, SDL_Renderer *renderer, Point camPos, float zoom);

SDL_Surface *component_create_wire_texture(Point V1, Point V2, float ang1, float ang2, float size, float thickness);

#endif //HOMEWORK_COMPONENT_H
