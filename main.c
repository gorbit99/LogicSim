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
#include "Parser.h"

int main(int argc, char **argv) {

	debugmalloc_log_file("debugmalloclog.txt");

	SDL_Window *window;
	SDL_Renderer *renderer;

	window_init(&window, &renderer, "Logic Simulator", 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	TTF_Font *font = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 80);

	ComponentData data = component_create(50, 50, "XNOR", 300, 15, font, renderer);
	ComponentData data2 = component_create(400, 600, "XNOR", 300, 15, font, renderer);
	ComponentData wire = component_create_wire_between(&data, &data2, 2, 0, 500, 15, renderer);

	FunctionData function = parser_load_function("res/AND.fun");
	parser_free_function(&function);

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

		component_render(&data, renderer, cameraPos, zoom);
		component_render(&data2, renderer, cameraPos, zoom);
		component_render(&wire, renderer, cameraPos, zoom);

		if (input_get_mouse_button(SDL_BUTTON_MIDDLE).isHeld) {
			cameraPos.x -= (float) input_get_mouse_delta_x() / zoom;
			cameraPos.y -= (float) input_get_mouse_delta_y() / zoom;
		}

		if (input_get_mouse_button(SDL_BUTTON_MIDDLE).isPressed) {
			SDL_FreeCursor(cursor);
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
			SDL_SetCursor(cursor);
			SDL_CaptureMouse(true);
		}
		if (input_get_mouse_button(SDL_BUTTON_MIDDLE).isReleased) {
			SDL_FreeCursor(cursor);
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
			SDL_SetCursor(cursor);
			SDL_CaptureMouse(false);
		}

		zoom *= powf(0.9f, -(float)input_get_mouse_wheel_y());
		if (zoom < 0.05) zoom = 0.05f;
		if (zoom > 1) zoom = 1;
		SDL_RenderSetScale(renderer, zoom, zoom);

		SDL_RenderPresent(renderer);
	}

	component_free_data(&data);
	component_free_data(&data2);
	component_free_data(&wire);
	window_cleanup(window, renderer);

	return 0;
}