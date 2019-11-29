#include "Search.h"
#include "debugmalloc.h"

ModuleList search_load_modules(char *moduleDir, TTF_Font *font, SDL_Renderer *renderer) {
	FileList files = dir_getfiles(moduleDir);
	ModuleList result;
	result.num = files.num / 3;
	result.modules = (struct ModuleData *) malloc(sizeof(struct ModuleData) * result.num);
	for (size_t i = 0; i < result.num; i++) {
		dir_remove_extension(files.files[i * 3]);
		result.modules[i].name = (char *) malloc(sizeof(char) * (strlen(files.files[i * 3]) + 1));
		strcpy(result.modules[i].name, files.files[i * 3]);
		SDL_Surface *surface = TTF_RenderText_Blended(
				font,
				result.modules[i].name,
				(SDL_Color) {255, 255, 255, 255});
		result.modules[i].text = SDL_CreateTextureFromSurface(renderer, surface);
		result.modules[i].w = surface->w;
		result.modules[i].h = surface->h;
		SDL_FreeSurface(surface);
	}
	dir_free_filelist(&files);
	return result;
}

void search_free_modules(ModuleList *moduleList) {
	for (size_t i = 0; i < moduleList->num; i++) {
		free(moduleList->modules[i].name);
		SDL_DestroyTexture(moduleList->modules[i].text);
	}
	free(moduleList->modules);
}

void search_render_module(struct ModuleData *module, int x, int y, SDL_Renderer *renderer) {
	SDL_Rect dst = {x, y, module->w, module->h};
	SDL_RenderCopy(renderer, module->text, NULL, &dst);
}

void search_render_modulelist(ModuleList *modules, SDL_Rect clipRect, char *search, int yOffset, SDL_Renderer *renderer) {
	SDL_RenderSetClipRect(renderer, &clipRect);
	size_t len = strlen(search);
	if (len == 0) {
		for (size_t i = 0; i < modules->num; i++) {
			search_render_module(&modules->modules[i],
			                     clipRect.x,
			                     clipRect.y - yOffset + (int) i * modules->modules[i].h,
			                     renderer);
		}
		SDL_RenderSetClipRect(renderer, NULL);
		return;
	}

	char *lowerSearch = (char *) malloc(sizeof(char) * (strlen(search) + 1));
	strcpy(lowerSearch, search);

	for (int i = 0; lowerSearch[i] != '\0'; i++)
		lowerSearch[i] = (char) tolower(lowerSearch[i]);
	int found = 0;
	for (size_t i = 0; i < modules->num; i++) {
		if (strcontains_nocase(modules->modules[i].name, lowerSearch)) {
			search_render_module(&modules->modules[i],
			                     clipRect.x,
			                     clipRect.y - yOffset + found * modules->modules[i].h,
			                     renderer);
			found++;
		}
	}
	free(lowerSearch);
	SDL_RenderSetClipRect(renderer, NULL);
}

SearchData search_create() {
	SearchData result;
	result.textInput = textinput_create();
	return result;
}

void search_start(SearchData *searchData, char *moduleDir, TTF_Font *font, SDL_Renderer *renderer) {
	searchData->selected = 0;
	searchData->modules = search_load_modules(moduleDir, font, renderer);
	searchData->yOffset = 0;
	searchData->searchOver = false;
	textinput_start();
}

SearchResult search_end(SearchData *searchData) {
	SearchResult result;
	textinput_end();
	textinput_clear(&searchData->textInput);
	result.selectedModule = NULL;
	if (searchData->selected < 0 || searchData->selected >= searchData->modules.num) {
		search_free_modules(&searchData->modules);
		return result;
	}
	result.selectedModule = (char *)malloc(sizeof(char) * (strlen(searchData->modules.modules[searchData->selected].name) + 1));
	strcpy(result.selectedModule, searchData->modules.modules[searchData->selected].name);
	search_free_modules(&searchData->modules);
	return result;
}

void search_handle_event(SearchData *searchData, SDL_Event *e) {
	if (e->type == SDL_KEYDOWN && e->key.keysym.scancode == SDL_SCANCODE_RETURN) {
		searchData->searchOver = true;
	} else if (e->type == SDL_KEYDOWN && e->key.keysym.scancode == SDL_SCANCODE_DOWN) {
		for (size_t i = searchData->selected + 1; i < searchData->modules.num; i++) {
			if (strcontains_nocase(searchData->modules.modules[i].name, searchData->textInput.text)) {
				searchData->selected = i;
				break;
			}
		}
	} else if (e->type == SDL_KEYDOWN && e->key.keysym.scancode == SDL_SCANCODE_UP) {
		for (int i = searchData->selected - 1; i >= 0; i--) {
			if (strcontains_nocase(searchData->modules.modules[i].name, searchData->textInput.text)) {
				searchData->selected = i;
				break;
			}
		}
	} else {
		if (textinput_handle_event(&searchData->textInput, e)) {
			searchData->selected = -1;
			for (size_t i = 0; i < searchData->modules.num; i++) {
				if (strcontains_nocase(searchData->modules.modules[i].name, searchData->textInput.text)) {
					searchData->selected = i;
					break;
				}
			}
			searchData->yOffset = 0;
		}
	}
}

void search_render(SearchData *searchData, SDL_Rect moduleClipRect, TTF_Font *font, int searchTextX, int searchTextY, SDL_Renderer *renderer) {
	textinput_update_graphic(&searchData->textInput, font);

	int offset = 0;
	for (int i = 0; i < searchData->selected; i++) {
		if (strcontains_nocase(searchData->modules.modules[i].name, searchData->textInput.text)) {
			offset++;
		}
	}
	if (moduleClipRect.y + searchData->modules.modules[0].h * (offset - searchData->yOffset) +
		searchData->modules.modules[0].h > moduleClipRect.y + moduleClipRect.h) {
		searchData->yOffset++;
	} 
	else if (moduleClipRect.y + searchData->modules.modules[0].h * (offset - searchData->yOffset) <
		moduleClipRect.y) {
		searchData->yOffset--;
	}

	search_render_modulelist(&searchData->modules, moduleClipRect, 
							 searchData->textInput.text, searchData->yOffset * searchData->modules.modules[0].h, renderer);
	if (searchData->selected != -1) {
		SDL_Rect selectedRect = {
			moduleClipRect.x,
			moduleClipRect.y + searchData->modules.modules[0].h * (offset - searchData->yOffset),
			moduleClipRect.w,
			searchData->modules.modules[0].h
		};
		SDL_SetRenderDrawColor(renderer, 200, 0, 255, 100);
		SDL_RenderFillRect(renderer, &selectedRect);
	}
	textinput_render(&searchData->textInput, font, searchTextX, searchTextY, renderer);
}

void search_free_result(SearchResult *result) {
	free(result->selectedModule);
}

void search_free(SearchData *searchData) {
	textinput_free(&searchData->textInput);
}