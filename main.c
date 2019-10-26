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
#include "Camera.h"

int main(int argc, char **argv) {
	debugmalloc_log_file("debugmalloclog.txt");

	SDL_Window *window;
	SDL_Renderer *renderer;

	window_init(&window, &renderer, "Logic Simulator", 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	TTF_Font *font = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 80);

	NodeVector vec = nodev_create(0);

	nodev_push_back(&vec, node_create_switch((Point){0, 0}, renderer));
	nodev_push_back(&vec, node_create_switch((Point){0, 300}, renderer));
	nodev_push_back(&vec, node_create_switch((Point){0, 600}, renderer));

	nodev_push_back(&vec, node_create("SPLITTER3", (Point){500, 300}, font, renderer));

	node_set_connection(&vec.nodes[0], 0, &vec.nodes[3], 0);
	node_set_connection(&vec.nodes[1], 0, &vec.nodes[3], 1);
	node_set_connection(&vec.nodes[2], 0, &vec.nodes[3], 2);

	nodev_update(&vec);

	SDL_Cursor *cursor = SDL_GetCursor();
	Camera camera;
	camera.position = (Point){0, 0};
	camera.zoom = 0.5f;

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

		SDL_SetRenderDrawColor(renderer,255,0, 0, 255);

		if (input_get_mouse_button(SDL_BUTTON_MIDDLE).isHeld)
			camera_move(&camera, (Vec){(float)input_get_mouse_delta_x(), (float)input_get_mouse_delta_y()});

		for (size_t i = 0; i < vec.count; i++)
			node_render(&vec.nodes[i], camera.position);

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

		for (int i = 0; i < 3; i++) {
			if (input_get_key(SDL_SCANCODE_1 + i).isPressed) {
				vec.nodes[i].outValues[0] = !vec.nodes[i].outValues[0];
				vec.nodes[i].dirty = true;
				nodev_update(&vec);
			}
		}

		camera_zoom(&camera, (float)input_get_mouse_wheel_y(), input_get_mouse_pos());
		SDL_RenderSetScale(renderer, camera.zoom, camera.zoom);

		SDL_RenderPresent(renderer);
	}
	window_cleanup(window, renderer);

	nodev_free(&vec);

	return 0;
}