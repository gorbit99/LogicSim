#include <stdio.h>
#define SDL_MAIN_HANDLED

#include "debugmalloc.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include "Graphics.h"
#include "Component.h"
#include "Input.h"
#include "NodeVector.h"
#include "Camera.h"
#include "GUIGraphics.h"
#include "Windowing.h"

int main(int argc, char **argv) {
	debugmalloc_log_file("debugmalloclog.txt");

	window_init_SDL();

	Window mainWindow = window_create(
		"Logic Simulator",
		640,
		480,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	TTF_Font *font = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 80);

	NodeVector vec = nodev_create(0);
	nodev_push_back(&vec, node_create_switch((Point){0, 0}, mainWindow.renderer));
	nodev_push_back(&vec, node_create_switch((Point){0, 300}, mainWindow.renderer));
	nodev_push_back(&vec, node_create_switch((Point){0, 600}, mainWindow.renderer));
	nodev_push_back(&vec, node_create_switch((Point){0, 900}, mainWindow.renderer));
	nodev_push_back(&vec, node_create("TEST", (Point){300, 450}, font, mainWindow.renderer));
	nodev_push_back(&vec, node_create_LED((Point){700, 0}, mainWindow.renderer));
	nodev_push_back(&vec, node_create_LED((Point){700, 300}, mainWindow.renderer));
	nodev_push_back(&vec, node_create_LED((Point){700, 600}, mainWindow.renderer));

	nodev_connect(&vec, 0, 0, 4, 0);
	nodev_connect(&vec, 1, 0, 4, 1);
	nodev_connect(&vec, 2, 0, 4, 2);
	nodev_connect(&vec, 3, 0, 4, 3);
	nodev_connect(&vec, 4, 0, 5, 0);
	nodev_connect(&vec, 4, 1, 6, 0);
	nodev_connect(&vec, 4, 2, 7, 0);

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
			}
			input_handle_event(&e);
		}

		SDL_SetRenderDrawColor(mainWindow.renderer, 0, 0, 0, 255);
		SDL_RenderClear(mainWindow.renderer);

		Point mouseWS = camera_screen_to_view(&camera, input_get_mouse_pos());

		if (input_get_mouse_button(SDL_BUTTON_MIDDLE).isHeld)
			camera_move(&camera, (Vec){(float)input_get_mouse_delta_x(), (float)input_get_mouse_delta_y()});

		if (input_get_mouse_button(SDL_BUTTON_LEFT).isPressed)
			nodev_check_clicks(&vec, mouseWS);

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
		nodev_render(&vec, camera.position);
		
		
		camera_zoom(&camera, (float)input_get_mouse_wheel_y(), input_get_mouse_pos());
		SDL_RenderSetScale(mainWindow.renderer, camera.zoom, camera.zoom);

		SDL_RenderPresent(mainWindow.renderer);
	}
	window_free(&mainWindow);

	window_quit_SDL();

	nodev_free(&vec);

	return 0;
}