#include <stdio.h>
#define SDL_MAIN_HANDLED

#include "debugmalloc.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include "Graphics.h"
#include "Component.h"
#include "Window.h"
#include "Input.h"

int main(int argc, char **argv) {

	debugmalloc_log_file("debugmalloclog.txt");

	SDL_Window *window;
	SDL_Renderer *renderer;

	window_init(&window, &renderer, "Logic Simulator", 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	TTF_Font *font = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 25);
	SDL_Texture *testT;
	SDL_Surface *test = component_load_graphic("res/XNOR.cmp", 100, 3, font);
	testT = SDL_CreateTextureFromSurface(renderer, test);
	SDL_FreeSurface(test);

	ComponentData data = component_create_data(50, 50, testT, 0);

	SDL_Cursor *cursor = SDL_GetCursor();
	Point cameraPos = {0, 0};
	float zoom = 1;

	bool quit = false;
	SDL_Event e;
	while (!quit) {
		input_reset_events();
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					SDL_CaptureMouse(true);
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					SDL_CaptureMouse(false);
					break;
			}
			input_handle_event(&e);
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		int w, h;
		SDL_QueryTexture(testT, NULL, NULL, &w, &h);
		component_render(&data, renderer, cameraPos);

		if (input_get_mouse_button(SDL_BUTTON_MIDDLE).isHeld) {
			cameraPos.x -= (float) input_get_mouse_delta_x();
			cameraPos.y -= (float) input_get_mouse_delta_y();
		}

		if (input_get_mouse_button(SDL_BUTTON_MIDDLE).isPressed) {
			SDL_FreeCursor(cursor);
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
			SDL_SetCursor(cursor);
		}
		if (input_get_mouse_button(SDL_BUTTON_MIDDLE).isReleased) {
			SDL_FreeCursor(cursor);
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
			SDL_SetCursor(cursor);
		}

		zoom *= powf(0.9f, -(float)input_get_mouse_wheel_y());
		if (zoom < 0.45) zoom = 0.45f;
		if (zoom > 5) zoom = 5;

		if (input_get_mouse_wheel_y() != 0 || input_get_key(SDL_SCANCODE_SPACE).isPressed) {
			SDL_DestroyTexture(testT);
			TTF_CloseFont(font);
			font = TTF_OpenFont("res/SourceCodePro-Regular.ttf", (int)(25 * zoom));
			SDL_Surface *surf = component_load_graphic("res/XNOR.cmp", 100 * zoom, 3 * zoom, font);
			testT = SDL_CreateTextureFromSurface(renderer, surf);
			SDL_FreeSurface(surf);
			component_update_texture(&data,testT);
		}

		SDL_RenderPresent(renderer);
	}

	window_cleanup(window, renderer);

	return 0;
}