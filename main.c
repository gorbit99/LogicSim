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

enum ProgramState {
	VIEWING_CIRCUIT,
	CHOOSING_COMPONENT,
	MOVING_COMPONENT
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
	Node *moved = NULL;

	NodeVector vec = nodev_create(0);

	nodev_push_back(&vec, node_create("switch", (Point){0, 0}, font, mainWindow.renderer));
	nodev_push_back(&vec, node_create("led", (Point){600, 300}, font, mainWindow.renderer));
	nodev_push_back(&vec, node_create("led", (Point){1200, 300}, font, mainWindow.renderer));
	nodev_push_back(&vec, node_create("led", (Point){300, 700}, font, mainWindow.renderer));
	nodev_connect(&vec, 0, 0, 1, 0);
	nodev_connect(&vec, 0, 0, 2, 0);
	nodev_connect(&vec, 0, 0, 3, 0);

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

		Point mouseWS = camera_screen_to_view(&camera, input_get_mouse_pos(&mainWindow.input));

		switch (state) {
			case VIEWING_CIRCUIT: {
				//Update
				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_LEFT).isPressed)
					nodev_check_clicks(&vec, mouseWS);
				camera_update(&camera, &mainWindow.input, mainWindow.renderer);

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
					moved = nodev_at(&vec, (int)vec.count - 1);
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
				moved->component.x = mouseWS.x;
				moved->component.y = mouseWS.y;

				//Transitions
				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_LEFT).isPressed) {
					state = VIEWING_CIRCUIT;
				}
				break;
			}
			default:
				break;
		}

		nodev_update(&vec);

		nodev_render(&vec, camera.position);

		//Test
		float dist;
		Point p = closest_point_on_wires(&vec, &dist, mouseWS);
		SDL_Rect rect = {
				p.x - camera.position.x - 3,
				p.y - camera.position.y - 3,
				6,
				6
		};
		SDL_SetRenderDrawColor(mainWindow.renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(mainWindow.renderer, &rect);
		//

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