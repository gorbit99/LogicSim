#include "Window.h"

#include "debugmalloc.h"

void __log_output_function(void *unused, int category, SDL_LogPriority priority, const char *message) {
	FILE *file = fopen("log.txt", "a");
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	fprintf(file, "----------%d-%02d-%02d %02d:%02d:%02d----------\n", tm.tm_year + 1900,
			tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	fprintf(file, "%s", message);
	fprintf(file, "\n");
}

bool window_init(SDL_Window **window, SDL_Renderer **renderer, const char *title, int w, int h, int windowFlags, int rendererFlags) {

	SDL_LogSetOutputFunction(__log_output_function, NULL);

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		log_error("Couldn't initialize SDL:\nError: %s\n", SDL_GetError());
		return false;
	}

	if (TTF_Init() != 0) {
		log_error("Couldn't initialize SDL_TTF:\nError: %s\n", SDL_GetError());
		return false;
	}

	*window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			w, h, windowFlags);
	if (!*window) {
		log_error("Couldn't create SDL Window:\nError: %s\n", SDL_GetError());
		SDL_Quit();
		return false;
	}

	*renderer = SDL_CreateRenderer(*window, -1, rendererFlags);
	if (!*renderer) {
		log_error("Couldn't create SDL Window:\nError: %s\n", SDL_GetError());
		SDL_DestroyWindow(*window);
		SDL_Quit();
		return false;
	}

	return true;
}

void window_cleanup(SDL_Window *window, SDL_Renderer *renderer) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
}
