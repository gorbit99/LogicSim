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

	nodev_push_back(&vec, node_create_switch((Point){0, 200}, renderer)); //A
	nodev_push_back(&vec, node_create_switch((Point){0, 700}, renderer)); //B
	nodev_push_back(&vec, node_create_switch((Point){0, 1200}, renderer)); //Ci
	nodev_push_back(&vec, node_create("SPLITTER", (Point){300, 200}, font, renderer));
	nodev_push_back(&vec, node_create("SPLITTER", (Point){400, 700}, font, renderer));
	nodev_push_back(&vec, node_create("SPLITTER", (Point){500, 1200}, font, renderer));
	nodev_push_back(&vec, node_create("XOR", (Point){800, 350}, font, renderer));
	nodev_push_back(&vec, node_create("XOR", (Point){1500, 500}, font, renderer));
	nodev_push_back(&vec, node_create("SPLITTER", (Point){1300, 450}, font, renderer));
	nodev_push_back(&vec, node_create("AND", (Point){1500, 900}, font, renderer));
	nodev_push_back(&vec, node_create("AND", (Point){1500, 1300}, font, renderer));
	nodev_push_back(&vec, node_create("OR", (Point){2000, 1100}, font, renderer));
	nodev_push_back(&vec, node_create_LED((Point){2500, 550}, renderer)); //S
	nodev_push_back(&vec, node_create_LED((Point){2500, 1150}, renderer)); //Co

	node_set_connection(&vec.nodes[0], 0, &vec.nodes[3], 0);
	node_set_connection(&vec.nodes[1], 0, &vec.nodes[4], 0);
	node_set_connection(&vec.nodes[2], 0, &vec.nodes[5], 0);
	node_set_connection(&vec.nodes[3], 0, &vec.nodes[6], 0);
	node_set_connection(&vec.nodes[4], 0, &vec.nodes[6], 1);
	node_set_connection(&vec.nodes[6], 0, &vec.nodes[8], 0);
	node_set_connection(&vec.nodes[8], 0, &vec.nodes[7], 0);
	node_set_connection(&vec.nodes[5], 0, &vec.nodes[7], 1);
	node_set_connection(&vec.nodes[7], 0, &vec.nodes[12], 0);
	node_set_connection(&vec.nodes[8], 1, &vec.nodes[9], 0);
	node_set_connection(&vec.nodes[5], 1, &vec.nodes[9], 1);
	node_set_connection(&vec.nodes[3], 1, &vec.nodes[10], 0);
	node_set_connection(&vec.nodes[4], 1, &vec.nodes[10], 1);
	node_set_connection(&vec.nodes[9], 0, &vec.nodes[11], 0);
	node_set_connection(&vec.nodes[10], 0, &vec.nodes[11], 1);
	node_set_connection(&vec.nodes[11], 0, &vec.nodes[13], 0);

	vec.nodes[0].outValues[0] = 0;
	vec.nodes[1].outValues[0] = 1;
	vec.nodes[2].outValues[0] = 0;

	node_update(&vec.nodes[0]);
	node_update(&vec.nodes[1]);
	node_update(&vec.nodes[2]);

	SDL_Cursor *cursor = SDL_GetCursor();
	Camera camera;

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

		if (input_get_mouse_button(SDL_BUTTON_MIDDLE).isHeld)
			camera_move(&camera, (Vec){input_get_mouse_delta_x(), input_get_mouse_delta_y()});

		for (size_t i = 0; i < vec.count; i++)
			node_render(&vec.nodes[i], camera.position, camera.zoom);

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
				node_update(&vec.nodes[i]);
			}
		}

		camera_zoom(&camera, input_get_mouse_wheel_y(), input_get_mouse_pos());
		SDL_RenderSetScale(renderer, camera.zoom, camera.zoom);

		SDL_RenderPresent(renderer);
	}
	window_cleanup(window, renderer);

	nodev_free(&vec);

	return 0;
}