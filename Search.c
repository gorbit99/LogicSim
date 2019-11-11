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
		for (int j = 0; result.modules[i].name[j] != '\0'; j++)
			result.modules[i].name[j] = (char) tolower(result.modules[i].name[j]);
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

void search_render(ModuleList *modules, SDL_Rect clipRect, char *search, int yOffset, SDL_Renderer *renderer) {
	//SDL_RenderSetClipRect(renderer, &clipRect);
	size_t len = strlen(search);
	if (len == 0) {
		for (size_t i = 0; i < modules->num; i++) {
			search_render_module(&modules->modules[i],
			                     clipRect.x,
			                     clipRect.y - yOffset + (int) i * modules->modules[i].h,
			                     renderer);
		}
		return;
	}

	char *lowerSearch = (char *) malloc(sizeof(char) * (strlen(search) + 1));
	strcpy(lowerSearch, search
	);
	for (int i = 0; lowerSearch[i] != '\0'; i++)
		lowerSearch[i] = (char) tolower(lowerSearch[i]);
	int found = 0;
	for (size_t i = 0; i < modules->num; i++) {
		if (strstr(modules->modules[i].name, lowerSearch)) {
			search_render_module(&modules->modules[i],
			                     clipRect.x,
			                     clipRect.y - yOffset + found * modules->modules[i].h,
			                     renderer);
			found++;
		}
	}
	//free(lowerSearch);
	//SDL_RenderSetClipRect(renderer, NULL);
}
