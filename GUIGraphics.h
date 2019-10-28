#ifndef HOMEWORK_GUIGRAPHICS_H
#define HOMEWORK_GUIGRAPHICS_H

#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include "Utility.h"

typedef enum SliceType {
	ST_CLAMP,
	ST_REPEAT
} SliceType;

typedef struct NSliceTexture {
	SDL_Texture *texture;
	int w, h;
	SliceType type;
	int x1, y1, x2, y2;
	int w1, w2, w3;
	int h1, h2, h3;
} NSliceTexture;

NSliceTexture guigfx_create_nslice(char *path, SliceType type, int x1, int y1, int x2, int y2, SDL_Renderer *renderer);

void guigfx_render_9slice(NSliceTexture *data, SDL_Rect rect, SDL_Renderer *renderer);

#endif //HOMEWORK_GUIGRAPHICS_H
