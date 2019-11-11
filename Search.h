#ifndef HOMEWORK_SEARCH_H
#define HOMEWORK_SEARCH_H

#include "Directory.h"
#include <SDL_ttf.h>

struct ModuleData {
	char *name;
	SDL_Texture *text;
	int w, h;
};

typedef struct ModuleList {
	 struct ModuleData *modules;

	size_t num;
} ModuleList;

ModuleList search_load_modules(char *moduleDir, TTF_Font *font, SDL_Renderer *renderer);

void search_free_modules(ModuleList *moduleList);

void search_render_module(struct ModuleData *module, int x, int y, SDL_Renderer *renderer);

void search_render(ModuleList *modules, SDL_Rect clipRect, char *search, int yOffset, SDL_Renderer *renderer);

#endif //HOMEWORK_SEARCH_H
