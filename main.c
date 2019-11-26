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
#include "Search.h"
#include "WireDrawing.h"
#include "Save.h"
#include "FileDialog.h"
#include "ConfigHandler.h"

enum ProgramState {
	VIEWING_CIRCUIT,
	CHOOSING_COMPONENT,
	MOVING_COMPONENT,
	DRAWING_WIRE,
	SIMULATION
} state;

int main(int argc, char **argv) {
	debugmalloc_log_file("debugmalloclog.txt");

	window_init_SDL();

	config_init();

	int w, h;
	w = config_get_int("screen-width");
	h = config_get_int("screen-height");

	Window mainWindow = window_create(
			"Logic Simulator",
			w,
			h,
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
	Node *moved = NULL;
	WireDrawing wireDrawing = {};

	char *lastOpened = config_get_string("last-opened");
	NodeVector vec;
	if (strcmp(lastOpened, "-") != 0)
		vec = load_vector(lastOpened, font, mainWindow.renderer);
	else
		vec = nodev_create(0);

	Camera camera;
	camera.position = (Point) {0, 0};
	camera.zoom = 0.25f;

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

		Point mouseWS = camera_screen_to_view(&camera, input_get_mouse_pos(&mainWindow.input));

		if (input_get_key(&mainWindow.input, SDL_SCANCODE_RETURN).isPressed)
			open_file_dialog(mainWindow.window, "Schematic Files\0*.sav\0\0", "Open Schematic", DT_SAVE);

		switch (state) {
			case VIEWING_CIRCUIT: {
				camera_update(&camera, &mainWindow.input, mainWindow.renderer);

				if (input_get_key(&mainWindow.input, SDL_SCANCODE_RCTRL).isHeld ||
				    input_get_key(&mainWindow.input, SDL_SCANCODE_LCTRL).isHeld) {
					if (input_get_key(&mainWindow.input, SDL_SCANCODE_S).isPressed) {
						char *path = open_file_dialog(mainWindow.window, "Schematic Files\0*.sav\0\0", "Save Schematic",
						                              DT_SAVE);
						save_vector(&vec, path);
						config_set_string("last-opened", path);
						free(path);
					}
					if (input_get_key(&mainWindow.input, SDL_SCANCODE_O).isPressed) {
						char *path = open_file_dialog(mainWindow.window, "Schematic Files\0*.sav\0\0", "Open Schematic",
						                              DT_OPEN);
						nodev_free(&vec);
						vec = load_vector(path, font, mainWindow.renderer);
						config_set_string("last-opened", path);
						free(path);
					}
				}


				//Transitions
				if (input_get_key(&mainWindow.input, SDL_SCANCODE_SPACE).isPressed) {
					state = CHOOSING_COMPONENT;
					window_show(&searchWindow);
					window_get_focus(&searchWindow);
					search_start(&search, "res/Modules", searchbarFont, searchWindow.renderer);
				}
				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_LEFT).isPressed &&
				    wiredrawing_start(&vec, mouseWS, &wireDrawing)) {
					state = DRAWING_WIRE;
					break;
				}
				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_LEFT).isPressed) {
					moved = NULL;
					for (size_t i = 0; i < vec.count; i++) {
						if (node_is_over(nodev_at(&vec, i), mouseWS)) {
							state = MOVING_COMPONENT;
							moved = nodev_at(&vec, i);
							break;
						}
					}
					if (moved != NULL)
						break;
				}
				if (input_get_key(&mainWindow.input, SDL_SCANCODE_ESCAPE).isPressed) {
					state = SIMULATION;
					break;
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
				guigfx_render_nslice(&textBoxTexture, (SDL_Rect) {275, 100, 200, 40}, searchWindow.renderer);
				guigfx_render_nslice(&panelTexture, (SDL_Rect) {10, 10, 230, 480}, searchWindow.renderer);
				search_render(&search, (SDL_Rect) {15, 15, 220, 470}, searchbarFont, 280, 105, searchWindow.renderer);

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
					nodev_push_back(&vec,
					                node_create(result.selectedModule, (Point) {0, 0}, font, mainWindow.renderer));
					moved = nodev_at(&vec, (int) vec.count - 1);
					search_free_result(&result);
					window_hide(&searchWindow);
					window_get_focus(&mainWindow);
					state = MOVING_COMPONENT;
				}
				break;
			}
			case MOVING_COMPONENT: {
				//Update
				camera_update(&camera, &mainWindow.input, mainWindow.renderer);
				nodev_reposition(&vec, moved, mouseWS);

				if (input_get_key(&mainWindow.input, SDL_SCANCODE_DELETE).isPressed) {
					nodev_delete(&vec, moved);
					moved = NULL;
				}

				//Transitions
				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_LEFT).isPressed || moved == NULL) {
					state = VIEWING_CIRCUIT;
				}
				break;
			}
			case DRAWING_WIRE: {
				//Update
				camera_update(&camera, &mainWindow.input, mainWindow.renderer);

				wiredrawing_update(&wireDrawing, &vec, mouseWS, camera.position, mainWindow.renderer);

				//Transitions
				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_LEFT).isReleased) {
					wiredrawing_end(&wireDrawing, &vec, mouseWS);
					state = VIEWING_CIRCUIT;
				}
				break;
			}
			case SIMULATION: {
				//Update
				camera_update(&camera, &mainWindow.input, mainWindow.renderer);
				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_LEFT).isPressed)
					nodev_check_clicks(&vec, mouseWS);
				//Transitions
				if (input_get_key(&mainWindow.input, SDL_SCANCODE_ESCAPE).isPressed) {
					state = VIEWING_CIRCUIT;
					break;
				}
				break;
			}
			default:
				break;
		}

		nodev_update(&vec);

		nodev_render(&vec, camera.position);

		SDL_RenderPresent(mainWindow.renderer);

		if (mainWindow.requestClose)
			quit = true;
	}

	config_set_int("screen-width", mainWindow.w);
	config_set_int("screen-height", mainWindow.h);
	config_save();
	config_free();

	window_free(&mainWindow);
	window_free(&searchWindow);
	search_free(&search);

	window_quit_SDL();

	nodev_free(&vec);

	return 0;
}