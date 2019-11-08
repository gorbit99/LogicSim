#include "Windowing.h"
#include "debugmalloc.h"

int __window_resize_handler(void *userdata, SDL_Event *event) {
    if (event->type == SDL_WINDOWEVENT)
        log_error("%d", event->window.event);
    if (event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_RESIZED) {
        SDL_RenderPresent(((Window *)userdata)->renderer);
        log_error("Resizing!\n");
    }
    return 0;
}

void __log_output_function(void *unused, int category, SDL_LogPriority priority, const char *message) {
	FILE *file = fopen("log.txt", "a");
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	if (priority == SDL_LOG_PRIORITY_CRITICAL)
		fprintf(file, "----------%d-%02d-%02d %02d:%02d:%02d----------\n", tm.tm_year + 1900,
			tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	fprintf(file, "%s", message);
	fprintf(file, "\n");
}

bool window_init_SDL() {
    SDL_LogSetOutputFunction(__log_output_function, NULL);

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		log_error("Couldn't initialize SDL:\nError: %s\n", SDL_GetError());
		return false;
	}

	if (TTF_Init() != 0) {
		log_error("Couldn't initialize SDL_TTF:\nError: %s\n", SDL_GetError());
		return false;
	}

    if (IMG_Init(IMG_INIT_PNG) != 0) {
		log_error("Couldn't initialize SDL_image:\nError: %s\n", SDL_GetError());
		return false;
	}
}

void window_quit_SDL() {
    IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

Window window_create(char *title, int w, int h, int windowFlags, int rendererFlags) {
    Window result;
    result.window = SDL_CreateWindow(title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        w,
        h,
        windowFlags);
    if (!result.window) {
        log_error("Couldn't create window!\n Error: %s", SDL_GetError());
        return (Window){};
    }
    result.renderer = SDL_CreateRenderer(result.window,
        -1,
        rendererFlags);

    SDL_AddEventWatch(__window_resize_handler, (void *)&result);
    return result;
}

void window_show(Window *window) {
    SDL_ShowWindow(window->window);
}

void window_hide(Window *window) {
    SDL_HideWindow(window->window);
}

void window_free(Window *window) {
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
}