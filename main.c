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
#include "NodeVector.h"

int main(int argc, char **argv) {
	debugmalloc_log_file("debugmalloclog.txt");

	SDL_Window *window;
	SDL_Renderer *renderer;

	window_init(&window, &renderer, "Logic Simulator", 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	TTF_Font *font = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 80);

	NodeVector vec = nodev_create(0);


	vec.nodes[0].inValues[0] = false;
	vec.nodes[0].inValues[1] = true;

	for (size_t i = 0; i < vec.count; i++)
		node_update(&vec.nodes[i]);

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

		if (input_get_mouse_button(SDL_BUTTON_MIDDLE).isHeld) {
			cameraPos.x -= (float) input_get_mouse_delta_x() / zoom;
			cameraPos.y -= (float) input_get_mouse_delta_y() / zoom;
		}

		for (size_t i = 0; i < vec.count; i++)
			node_render(&vec.nodes[i], cameraPos, zoom);

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
		if (input_get_key(SDL_SCANCODE_SPACE).isPressed) {
			node_set_inval(&vec.nodes[0], 0, rand() % 2);
			node_set_inval(&vec.nodes[0], 1, rand() % 2);
			node_set_inval(&vec.nodes[2], 0, rand() % 2);
			node_set_inval(&vec.nodes[2], 1, rand() % 2);
		}

		zoom *= powf(0.9f, -(float)input_get_mouse_wheel_y());
		if (zoom < 0.05) zoom = 0.05f;
		if (zoom > 1) zoom = 1;
		SDL_RenderSetScale(renderer, zoom, zoom);

		SDL_RenderPresent(renderer);
	}
	window_cleanup(window, renderer);

	nodev_free(&vec);

	return 0;
}