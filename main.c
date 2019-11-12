#include <stdio.h>

#define SDL_MAIN_HANDLED

#include "debugmalloc.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include "Graphics.h"
#include "Input.h"
#include "NodeVector.h"
#include "Camera.h"
#include "GUIGraphics.h"
#include "Windowing.h"
#include "TextInput.h"
#include "Search.h"

enum ProgramState {
	VIEWING_CIRCUIT,
	CHOOSING_COMPONENT
} state;

int main(int argc, char **argv) {
	debugmalloc_log_file("debugmalloclog.txt");

	window_init_SDL();

	Window mainWindow = window_create(
			"Logic Simulator",
			640,
			480,
			(unsigned) SDL_WINDOW_SHOWN | (unsigned) SDL_WINDOW_RESIZABLE,
			(unsigned) SDL_RENDERER_ACCELERATED | (unsigned) SDL_RENDERER_PRESENTVSYNC
	);

	Window searchWindow = window_create(
			"Search for component",
			500,
			500,
			SDL_WINDOW_HIDDEN,
			(unsigned) SDL_RENDERER_ACCELERATED | (unsigned) SDL_RENDERER_PRESENTVSYNC
	);

	state = VIEWING_CIRCUIT;

	TTF_Font *font = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 80);
	TTF_Font *searchbarFont = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 20);

	NSliceTexture textBoxTexture = guigfx_create_nslice(
			"res/GUI/Textbox.png",
			ST_CLAMP,
			9,
			9,
			181,
			181,
			searchWindow.renderer
	);

	NSliceTexture panelTexture = guigfx_create_nslice(
			"res/GUI/PanelBG.png",
			ST_CLAMP,
			9,
			9,
			181,
			181,
			searchWindow.renderer
	);

	SearchData search = search_create();

	NodeVector vec = nodev_create(0);
	nodev_push_back(&vec, node_create_switch((Point) {0, 0}, mainWindow.renderer));
	nodev_push_back(&vec, node_create_switch((Point) {0, 300}, mainWindow.renderer));
	nodev_push_back(&vec, node_create_switch((Point) {0, 600}, mainWindow.renderer));
	nodev_push_back(&vec, node_create_switch((Point) {0, 900}, mainWindow.renderer));
	nodev_push_back(&vec, node_create("TEST", (Point) {300, 450}, font, mainWindow.renderer));
	nodev_push_back(&vec, node_create_LED((Point) {700, 0}, mainWindow.renderer));
	nodev_push_back(&vec, node_create_LED((Point) {700, 300}, mainWindow.renderer));
	nodev_push_back(&vec, node_create_LED((Point) {700, 600}, mainWindow.renderer));

	nodev_connect(&vec, 0, 0, 4, 0);
	nodev_connect(&vec, 1, 0, 4, 1);
	nodev_connect(&vec, 2, 0, 4, 2);
	nodev_connect(&vec, 3, 0, 4, 3);
	nodev_connect(&vec, 4, 0, 5, 0);
	nodev_connect(&vec, 4, 1, 6, 0);
	nodev_connect(&vec, 4, 2, 7, 0);

	SDL_Cursor *cursor = SDL_GetCursor();
	Camera camera;
	camera.position = (Point) {0, 0};
	camera.zoom = 1;

	bool quit = false;
	SDL_Event e;
	while (!quit) {
		window_begin_event_handling(&mainWindow);
		while (SDL_PollEvent(&e)) {
			window_handle_event(&mainWindow, &e);
			window_handle_event(&searchWindow, &e);
			if (state == CHOOSING_COMPONENT)
				search_handle_event(&search, &e);
		}
		SDL_SetRenderDrawColor(mainWindow.renderer, 0, 0, 0, 255);
		SDL_RenderClear(mainWindow.renderer);

		switch (state) {
			case VIEWING_CIRCUIT: {
				//Update
				Point mouseWS = camera_screen_to_view(&camera, input_get_mouse_pos(&mainWindow.input));

				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_MIDDLE).isHeld)
					camera_move(&camera, (Vec) {(float) input_get_mouse_delta_x(&mainWindow.input),
					                            (float) input_get_mouse_delta_y(&mainWindow.input)});

				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_LEFT).isPressed)
					nodev_check_clicks(&vec, mouseWS);

				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_MIDDLE).isPressed) {
					SDL_FreeCursor(cursor);
					cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
					SDL_SetCursor(cursor);
					SDL_CaptureMouse(true);
				}
				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_MIDDLE).isReleased) {
					SDL_FreeCursor(cursor);
					cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
					SDL_SetCursor(cursor);
					SDL_CaptureMouse(false);
				}

				camera_zoom(&camera, (float) input_get_mouse_wheel_y(&mainWindow.input),
				            input_get_mouse_pos(&mainWindow.input));
				SDL_RenderSetScale(mainWindow.renderer, camera.zoom, camera.zoom);

				//Transitions
				if (input_get_key(&mainWindow.input, SDL_SCANCODE_SPACE).isPressed) {
					state = CHOOSING_COMPONENT;
					window_show(&searchWindow);
					window_get_focus(&searchWindow);
					search_start(&search, "res/Modules", searchbarFont, searchWindow.renderer);
				}
				break;
			}
			case CHOOSING_COMPONENT: {
				//Update
				if (mainWindow.keyboardFocus) {
					mainWindow.keyboardFocus = false;
					window_get_focus(&searchWindow);
				}

				//Graphics
				SDL_SetRenderDrawColor(searchWindow.renderer, 50, 50, 50, 255);
				SDL_RenderClear(searchWindow.renderer);
				guigfx_render_nslice(&textBoxTexture, (SDL_Rect){275, 100, 200, 40}, searchWindow.renderer);
				guigfx_render_nslice(&panelTexture, (SDL_Rect){10, 10, 230, 480}, searchWindow.renderer);

				search_render(&search, (SDL_Rect){15, 15, 220, 470}, searchbarFont, 280, 105, searchWindow.renderer);
				
				SDL_RenderPresent(searchWindow.renderer);

				//Transitions
				if (searchWindow.requestClose) {
					searchWindow.requestClose = false;
					window_hide(&searchWindow);
					state = VIEWING_CIRCUIT;
					window_get_focus(&mainWindow);
					SearchResult result = search_end(&search);
					search_free_result(&result);
				}
				if (search.searchOver) {
					SearchResult result = search_end(&search);
					nodev_push_back(&vec, node_create(result.selectedModule, (Point){0, 0}, font, mainWindow.renderer));
					search_free_result(&result);
					window_hide(&searchWindow);
					window_get_focus(&mainWindow);
					state = VIEWING_CIRCUIT;
				}
				break;
			}
			default:
				break;
		}

		nodev_render(&vec, camera.position);

		SDL_RenderPresent(mainWindow.renderer);

		if (mainWindow.requestClose)
			quit = true;
	}
	window_free(&mainWindow);
	window_free(&searchWindow);

	search_free(&search);

	window_quit_SDL();

	nodev_free(&vec);

	return 0;
}