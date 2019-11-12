#ifndef HOMEWORK_SEARCH_H
#define HOMEWORK_SEARCH_H

#include "Directory.h"
#include <SDL_ttf.h>
#include "TextInput.h"

struct ModuleData {
	char *name;
	SDL_Texture *text;
	int w, h;
};

typedef struct ModuleList {
	struct ModuleData *modules;
	size_t num;
} ModuleList;

typedef struct SearchData {
	ModuleList modules;
	int selected;
	TextInput textInput;
	int yOffset;
	bool searchOver;
} SearchData;

typedef struct SearchResult {
	char *selectedModule;
} SearchResult;

ModuleList search_load_modules(char *moduleDir, TTF_Font *font, SDL_Renderer *renderer);

void search_free_modules(ModuleList *moduleList);

void search_render_module(struct ModuleData *module, int x, int y, SDL_Renderer *renderer);

void search_render_modulelist(ModuleList *modules, SDL_Rect clipRect, char *search, int yOffset, SDL_Renderer *renderer);

SearchData search_create();

void search_start(SearchData *searchData, char *moduleDir, TTF_Font *font, SDL_Renderer *renderer);

void search_free_result(SearchResult *result);

void search_handle_event(SearchData *searchData, SDL_Event *e);

void search_render(SearchData *searchData, SDL_Rect moduleClipRect, TTF_Font *font, int searchTextX, int searchTextY, SDL_Renderer *renderer);

SearchResult search_end(SearchData *searchData);

void search_free(SearchData *searchData);

#endif //HOMEWORK_SEARCH_H
