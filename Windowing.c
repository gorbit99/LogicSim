#include "Windowing.h"
#include "debugmalloc.h"

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

	IMG_Init(IMG_INIT_PNG);

	SDL_StopTextInput();
}

void window_quit_SDL() {
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

Window window_create(char *title, int w, int h, unsigned int windowFlags, unsigned int rendererFlags) {
	Window result;
	result.window = SDL_CreateWindow(title,
	                                 SDL_WINDOWPOS_UNDEFINED,
	                                 SDL_WINDOWPOS_UNDEFINED,
	                                 w,
	                                 h,
	                                 windowFlags);
	if (!result.window) {
		log_error("Couldn't create window!\n Error: %s", SDL_GetError());
		return (Window) {};
	}
	result.renderer = SDL_CreateRenderer(result.window,
	                                     -1,
	                                     rendererFlags);
	if (!result.renderer) {
		log_error("Couldn't create renderer!\n Error: %s", SDL_GetError());
		return (Window) {};
	}

	result.w = w;
	result.h = h;
	result.id = SDL_GetWindowID(result.window);
	result.shown = (windowFlags & (unsigned) SDL_WINDOW_SHOWN) != 0;
	result.keyboardFocus = (windowFlags & (unsigned) SDL_WINDOW_INPUT_FOCUS);
	result.mouseFocus = (windowFlags & (unsigned) SDL_WINDOW_MOUSE_FOCUS);
	result.requestClose = false;
    SDL_SetRenderDrawBlendMode(result.renderer, SDL_BLENDMODE_BLEND);

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

void window_begin_event_handling(Window *window) {
	input_reset_events(&window->input);
}

void window_handle_event(Window *window, SDL_Event *e) {
	if (e->window.windowID != window->id)
		return;

	if (e->type == SDL_WINDOWEVENT) {
		switch (e->window.event) {
			case SDL_WINDOWEVENT_SHOWN:
				window->shown = true;
				break;
			case SDL_WINDOWEVENT_HIDDEN:
				window->shown = false;
				break;
			case SDL_WINDOWEVENT_EXPOSED:
				SDL_RenderPresent(window->renderer);
				break;
			case SDL_WINDOWEVENT_MOVED:
				break;
			case SDL_WINDOWEVENT_RESIZED:
				break;
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				window->w = e->window.data1;
				window->h = e->window.data2;
				break;
			case SDL_WINDOWEVENT_MINIMIZED:
				break;
			case SDL_WINDOWEVENT_MAXIMIZED:
				window->maximized = true;
				break;
			case SDL_WINDOWEVENT_RESTORED:
				window->maximized = false;
				break;
			case SDL_WINDOWEVENT_ENTER:
				window->mouseFocus = true;
				break;
			case SDL_WINDOWEVENT_LEAVE:
				window->mouseFocus = false;
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				window->keyboardFocus = true;
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				window->keyboardFocus = false;
				break;
			case SDL_WINDOWEVENT_CLOSE:
				window->requestClose = true;
				break;
			default:
				break;
		}
	}

	input_handle_event(&window->input, e);
}

void window_get_focus(Window *window) {
	SDL_RaiseWindow(window->window);
}

int window_get_width(Window *window) {
	return 0;
}

void window_maximize(Window *window) {
	window->maximized = true;
	SDL_MaximizeWindow(window->window);
}