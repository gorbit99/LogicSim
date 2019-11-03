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
#include "GUIGraphics.h"

int main(int argc, char **argv) {
	debugmalloc_log_file("debugmalloclog.txt");

	SDL_Window *window;
	SDL_Renderer *renderer;

	window_init(&window, &renderer, "Logic Simulator", 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	TTF_Font *font = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 80);

	NodeVector vec = nodev_create(0);
	nodev_push_back(&vec, node_create_switch((Point){0, 0}, renderer)); //0
	nodev_push_back(&vec, node_create_switch((Point){0, 400}, renderer)); //1
	nodev_push_back(&vec, node_create_switch((Point){0, 800}, renderer)); //2
	nodev_push_back(&vec, node_create("AND3", (Point){500, 100}, font, renderer)); //3
	nodev_push_back(&vec, node_create("AND3", (Point){500, 700}, font, renderer)); //4
	nodev_push_back(&vec, node_create("NOR", (Point){1000, 200}, font, renderer)); //5
	nodev_push_back(&vec, node_create("NOR", (Point){1000, 600}, font, renderer)); //6
	nodev_push_back(&vec, node_create("SPLITTER", (Point){200, 400}, font, renderer)); //7
	nodev_push_back(&vec, node_create("SPLITTER", (Point){1500, 200}, font, renderer)); //8
	nodev_push_back(&vec, node_create("SPLITTER", (Point){1500, 600}, font, renderer)); //9
	nodev_push_back(&vec, node_create("SPLITTER", (Point){1800, 200}, font, renderer)); //10
	nodev_push_back(&vec, node_create("SPLITTER", (Point){1800, 600}, font, renderer)); //11
	nodev_push_back(&vec, node_create_LED((Point){2000, 200}, renderer)); //12
	nodev_push_back(&vec, node_create_LED((Point){2000, 600}, renderer)); //13

	nodev_connect(&vec, 1, 0, 7, 0);
	nodev_connect(&vec, 5, 0, 8, 0);
	nodev_connect(&vec, 6, 0, 9, 0);
	nodev_connect(&vec, 8, 0, 10, 0);
	nodev_connect(&vec, 9, 1, 11, 0);

	nodev_connect(&vec, 11, 0, 3, 0);
	nodev_connect(&vec, 0, 0, 3, 1);
	nodev_connect(&vec, 7, 0, 3, 2);

	nodev_connect(&vec, 7, 1, 4, 0);
	nodev_connect(&vec, 2, 0, 4, 1);
	nodev_connect(&vec, 10, 1, 4, 2);

	nodev_connect(&vec, 3, 0, 5, 0);
	nodev_connect(&vec, 9, 0, 5, 1);

	nodev_connect(&vec, 8, 1, 6, 0);
	nodev_connect(&vec, 4, 0, 6, 1);

	nodev_connect(&vec, 10, 0, 12, 0);
	nodev_connect(&vec, 11, 1, 13, 0);

	nodev_update_all(&vec);
	nodev_update_all(&vec);

	SDL_Cursor *cursor = SDL_GetCursor();
	Camera camera;
	camera.position = (Point){0, 0};
	camera.zoom = 1;

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

		Point mouseWS = camera_screen_to_view(&camera, input_get_mouse_pos());

		if (input_get_mouse_button(SDL_BUTTON_MIDDLE).isHeld)
			camera_move(&camera, (Vec){(float)input_get_mouse_delta_x(), (float)input_get_mouse_delta_y()});

		if (input_get_mouse_button(SDL_BUTTON_LEFT).isPressed) {
			for (int i = 0; i < vec.count; i++) {
				if (nodev_at(&vec, i)->component.type == CT_SWITCH)
					if (node_is_over(nodev_at(&vec, i), mouseWS)) {
						nodev_switch(&vec, i);
					}
			}
		}

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

		camera_zoom(&camera, (float)input_get_mouse_wheel_y(), input_get_mouse_pos());
		SDL_RenderSetScale(renderer, camera.zoom, camera.zoom);

		SDL_RenderPresent(renderer);
	}
	window_cleanup(window, renderer);

	nodev_free(&vec);

	return 0;
}