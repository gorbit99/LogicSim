#include "debugmalloc.h"

#include "NodeVector.h"
#include "Camera.h"
#include "Windowing.h"
#include "Search.h"
#include "WireDrawing.h"
#include "Save.h"
#include "FileDialog.h"
#include "ConfigHandler.h"
#include "GUI.h"

enum ProgramState {
	VIEWING_CIRCUIT,
	CHOOSING_COMPONENT,
	MOVING_COMPONENT,
	DRAWING_WIRE,
	SIMULATION,
	SAVE_AS_MODULE
} state;

int main(int argc, char **argv) {
	debugmalloc_log_file("debugmalloclog.txt");

	window_init_SDL();
	config_init();

	int w, h;
	w = config_get_int("screen-width");
	h = config_get_int("screen-height");
	bool maximized = config_get_bool("maximized");

	unsigned int windowFlags = (unsigned) SDL_WINDOW_SHOWN |
	                           (unsigned) SDL_WINDOW_RESIZABLE;
	if (maximized)
		windowFlags |= (unsigned) SDL_WINDOW_MAXIMIZED;

	SDLWindow mainWindow = window_create(
			"Logic Simulator",
			w,
			h,
			windowFlags,
			(unsigned) SDL_RENDERER_ACCELERATED | (unsigned) SDL_RENDERER_PRESENTVSYNC
	);

	SDLWindow searchWindow = window_create(
			"Search for component",
			500,
			500,
			SDL_WINDOW_HIDDEN,
			(unsigned) SDL_RENDERER_ACCELERATED | (unsigned) SDL_RENDERER_PRESENTVSYNC
	);

	SDLWindow modulizeWindow = window_create(
			"Save as Module",
			500,
			50,
			(unsigned)SDL_WINDOW_HIDDEN | (unsigned)SDL_WINDOW_BORDERLESS,
			(unsigned) SDL_RENDERER_ACCELERATED | (unsigned) SDL_RENDERER_PRESENTVSYNC
	);

	state = VIEWING_CIRCUIT;

	TTF_Font *font = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 80);
	TTF_Font *searchbarFont = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 20);
	TTF_Font *modulizeFont = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 20);

	NSliceTexture textBoxTexture = guigfx_create_nslice(
			"res/GUI/Textbox.png",
			ST_CLAMP,
			9,
			9,
			181,
			181,
			searchWindow.renderer
	);

	NSliceTexture textBoxTextureModulize = guigfx_create_nslice(
			"res/GUI/Textbox.png",
			ST_CLAMP,
			9,
			9,
			181,
			181,
			modulizeWindow.renderer
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

	Camera camera = {(Point) {0, 0}, 0.25f};

	TextInput modulizeTI = textinput_create();

	Button newFileB = button_create((SDL_Rect) {8, 28, 32, 32}, "res/GUI/NewFile.png", mainWindow.renderer);
	Button openFileB = button_create((SDL_Rect) {8, 88, 32, 32}, "res/GUI/OpenFile.png", mainWindow.renderer);
	Button saveFileB = button_create((SDL_Rect) {8, 148, 32, 32}, "res/GUI/SaveFile.png", mainWindow.renderer);
	Button modulizeB = button_create((SDL_Rect) {8, 208, 32, 32}, "res/GUI/Modulize.png", mainWindow.renderer);

	Button addModuleB = button_create((SDL_Rect) {-8, 28, 32, 32}, "res/GUI/AddModule.png", mainWindow.renderer);
	Button simulateB = button_create((SDL_Rect) {-8, 88, 32, 32}, "res/GUI/Simulate.png", mainWindow.renderer);
	Button drawB = button_create((SDL_Rect) {-8, 88, 32, 32}, "res/GUI/Build.png", mainWindow.renderer);

	bool quit = false;
	SDL_Event e;
	while (!quit) {
		window_begin_event_handling(&mainWindow);
		window_begin_event_handling(&searchWindow);
		window_begin_event_handling(&modulizeWindow);
		while (SDL_PollEvent(&e)) {
			window_handle_event(&mainWindow, &e);
			window_handle_event(&searchWindow, &e);
			window_handle_event(&modulizeWindow, &e);
			if (state == CHOOSING_COMPONENT)
				search_handle_event(&search, &e);
			if (state == SAVE_AS_MODULE)
				textinput_handle_event(&modulizeTI, &e);
		}
		if (state != SIMULATION)
			SDL_SetRenderDrawColor(mainWindow.renderer, 0, 0, 0, 255);
		else
			SDL_SetRenderDrawColor(mainWindow.renderer, 40, 0, 25, 255);

		SDL_RenderClear(mainWindow.renderer);
		const SDL_Rect clipRect = {
				50,
				0,
				mainWindow.w - 100,
				mainWindow.h
		};
		SDL_RenderSetScale(mainWindow.renderer, 1, 1);
		SDL_RenderSetClipRect(mainWindow.renderer, &clipRect);

		Point mousePos = input_get_mouse_pos(&mainWindow.input);
		Point mouseWS = camera_screen_to_view(&camera, mousePos);

		if (input_get_key(&mainWindow.input, SDL_SCANCODE_RETURN).isPressed)
			open_file_dialog(mainWindow.window, "Schematic Files\0*.sch\0\0", "Open Schematic", DT_SAVE);

		switch (state) {
			case VIEWING_CIRCUIT: {
				camera_update(&camera, &mainWindow.input);

				if (input_get_mod(&mainWindow.input, MOD_CTRL).isHeld) {
					if (input_get_key(&mainWindow.input, SDL_SCANCODE_S).isPressed) {
						char *path = open_file_dialog(mainWindow.window, "Schematic Files\0*.sch\0\0", "Save Schematic",
						                              DT_SAVE);
						if (path != NULL) {
							save_vector(&vec, path);
							config_set_string("last-opened", path);
							free(path);
						}
					}
					if (input_get_key(&mainWindow.input, SDL_SCANCODE_O).isPressed) {
						char *path = open_file_dialog(mainWindow.window, "Schematic Files\0*.sch\0\0", "Open Schematic",
						                              DT_OPEN);
						if (path != NULL) {
							nodev_free(&vec);
							vec = load_vector(path, font, mainWindow.renderer);
							config_set_string("last-opened", path);
							free(path);
						}
					}
				}

				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_LEFT).isPressed) {
					if (button_is_over(&newFileB, mousePos)) {
						nodev_free(&vec);
						vec = nodev_create(0);
						break;
					}
					if (button_is_over(&saveFileB, mousePos)) {
						char *path = open_file_dialog(mainWindow.window, "Schematic Files\0*.sch\0\0", "Save Schematic",
						                              DT_SAVE);
						if (path != NULL) {
							save_vector(&vec, path);
							config_set_string("last-opened", path);
							free(path);
						}
						break;
					}
					if (button_is_over(&openFileB, mousePos)) {
						char *path = open_file_dialog(mainWindow.window, "Schematic Files\0*.sch\0\0", "Open Schematic",
						                              DT_OPEN);
						if (path != NULL) {
							nodev_free(&vec);
							vec = load_vector(path, font, mainWindow.renderer);
							config_set_string("last-opened", path);
							free(path);
						}
						break;
					}
				}

				//Transitions
				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_LEFT).isPressed) {
					if (button_is_over(&addModuleB, mousePos)) {
						state = CHOOSING_COMPONENT;
						window_show(&searchWindow);
						window_get_focus(&searchWindow);
						search_start(&search, "res/Modules", searchbarFont, searchWindow.renderer);
						break;
					}
					if (button_is_over(&simulateB, mousePos)) {
						state = SIMULATION;
						break;
					}
					if (button_is_over(&modulizeB, mousePos)) {
						state = SAVE_AS_MODULE;
						window_show(&modulizeWindow);
						window_get_focus(&modulizeWindow);
						textinput_start();
						textinput_clear(&modulizeTI);
						break;
					}
				}
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
				if (input_get_mod(&mainWindow.input, MOD_CTRL).isHeld &&
				    input_get_key(&mainWindow.input, SDL_SCANCODE_M).isPressed) {
					state = SAVE_AS_MODULE;
					window_show(&modulizeWindow);
					window_get_focus(&modulizeWindow);
					textinput_start();
					textinput_clear(&modulizeTI);
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
					if (result.selectedModule != NULL) {
						nodev_push_back(&vec,
					                node_create(result.selectedModule, (Point) {0, 0}, font, mainWindow.renderer));
						moved = nodev_at(&vec, (int) vec.count - 1);
						search_free_result(&result);
						state = MOVING_COMPONENT;
					} else
						state = VIEWING_CIRCUIT;
					window_hide(&searchWindow);
					window_get_focus(&mainWindow);
				}
				break;
			}
			case MOVING_COMPONENT: {
				//Update
				camera_update(&camera, &mainWindow.input);
				nodev_reposition(&vec, moved, mouseWS);

				if (input_get_key(&mainWindow.input, SDL_SCANCODE_DELETE).isPressed) {
					nodev_delete(&vec, moved);
					moved = NULL;
				}

				//Transitions
				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_LEFT).isPressed || moved == NULL) {
					if (input_get_mod(&mainWindow.input, MOD_CTRL).isHeld && moved != NULL) {
						nodev_push_back(&vec, node_create(moved->component.name,
						                                  (Point) {moved->component.x, moved->component.y}, font,
						                                  mainWindow.renderer));
						moved = nodev_at(&vec, (int) vec.count - 1);
					} else
						state = VIEWING_CIRCUIT;
				}
				break;
			}
			case DRAWING_WIRE: {
				//Update
				camera_update(&camera, &mainWindow.input);

				SDL_RenderSetScale(mainWindow.renderer, camera.zoom, camera.zoom);
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
				camera_update(&camera, &mainWindow.input);
				if (input_get_mouse_button(&mainWindow.input, SDL_BUTTON_LEFT).isPressed) {
					if (button_is_over(&drawB, mousePos)) {
						state = VIEWING_CIRCUIT;
						break;
					}
					nodev_check_clicks(&vec, mouseWS);
				}
				//Transitions
				if (input_get_key(&mainWindow.input, SDL_SCANCODE_ESCAPE).isPressed) {
					state = VIEWING_CIRCUIT;
					break;
				}
				break;
			}
			case SAVE_AS_MODULE: {
				//Update
				if (mainWindow.keyboardFocus) {
					mainWindow.keyboardFocus = false;
					window_get_focus(&modulizeWindow);
				}

				//Graphics
				SDL_SetRenderDrawColor(modulizeWindow.renderer, 50, 50, 50, 255);
				SDL_RenderClear(modulizeWindow.renderer);
				guigfx_render_nslice(&textBoxTextureModulize, (SDL_Rect) {0, 0, 500, 50}, modulizeWindow.renderer);
				textinput_update_graphic(&modulizeTI, modulizeFont);
				textinput_render(&modulizeTI, modulizeFont, 10, 10, modulizeWindow.renderer);
				SDL_RenderPresent(modulizeWindow.renderer);

				//Transitions
				if (modulizeWindow.requestClose) {
					modulizeWindow.requestClose = false;
					window_hide(&modulizeWindow);
					state = VIEWING_CIRCUIT;
					window_get_focus(&mainWindow);
					textinput_end();
					break;
				}
				if (input_get_key(&modulizeWindow.input, SDL_SCANCODE_RETURN).isPressed) {
					bool found = false;
					for (size_t i = 0; i < vec.count; i++) {
						if (strcmp_nocase(nodev_at(&vec, i)->component.name, modulizeTI.text)) {
							const SDL_MessageBoxButtonData buttons[] = {
									{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "OK"}
							};
							const SDL_MessageBoxColorScheme colorScheme = {
									{
											{0, 0, 0},
											{255, 255, 255},
											{128, 128, 128},
											{40, 40, 40},
											{80, 40, 80}
									}
							};
							const SDL_MessageBoxData messageBoxData = {
									SDL_MESSAGEBOX_ERROR,
									mainWindow.window,
									"Error",
									"The schematics contains the module\n you want to save to!",
									SDL_arraysize(buttons),
									buttons,
									&colorScheme
							};
							int buttonId = -1;
							SDL_ShowMessageBox(&messageBoxData, &buttonId);
							found = true;
							break;
						}
					}
					if (!found) {
						save_as_module(&vec, modulizeTI.text);
					}
					window_hide(&modulizeWindow);
					state = VIEWING_CIRCUIT;
					window_get_focus(&mainWindow);
					textinput_end();
					break;
				}
				if (input_get_key(&modulizeWindow.input, SDL_SCANCODE_ESCAPE).isPressed) {
					window_hide(&modulizeWindow);
					state = VIEWING_CIRCUIT;
					window_get_focus(&mainWindow);
					textinput_end();
					break;
				}
				break;
			}
			default:
				break;
		}

		nodev_update(&vec);

		SDL_RenderSetClipRect(mainWindow.renderer, NULL);

		SDL_RenderSetScale(mainWindow.renderer, 1, 1);
		SDL_SetRenderDrawColor(mainWindow.renderer, 25, 25, 25, 255);
		SDL_Rect sidebarRect = {
				0,
				0,
				50,
				mainWindow.h
		};
		SDL_RenderFillRect(mainWindow.renderer, &sidebarRect);
		sidebarRect.x = mainWindow.w - 50;
		SDL_RenderFillRect(mainWindow.renderer, &sidebarRect);
		button_render(&newFileB, mainWindow.renderer, mousePos);
		button_render(&openFileB, mainWindow.renderer, mousePos);
		button_render(&saveFileB, mainWindow.renderer, mousePos);
		button_render(&modulizeB, mainWindow.renderer, mousePos);

		addModuleB.rect.x = mainWindow.w - 8 - addModuleB.rect.w;
		simulateB.rect.x = mainWindow.w - 8 - simulateB.rect.w;
		drawB.rect.x = mainWindow.w - 8 - drawB.rect.w;
		button_render(&addModuleB, mainWindow.renderer, mousePos);
		if (state != SIMULATION)
			button_render(&simulateB, mainWindow.renderer, mousePos);
		else
			button_render(&drawB, mainWindow.renderer, mousePos);

		SDL_RenderSetClipRect(mainWindow.renderer, &clipRect);
		SDL_RenderSetScale(mainWindow.renderer, camera.zoom, camera.zoom);
		nodev_render(&vec, camera.position);
		SDL_RenderSetClipRect(mainWindow.renderer, NULL);

		SDL_RenderPresent(mainWindow.renderer);

		if (mainWindow.requestClose)
			quit = true;
	}

	config_set_int("screen-width", mainWindow.w);
	config_set_int("screen-height", mainWindow.h);
	config_set_bool("maximized", mainWindow.maximized);
	config_save();
	config_free();

	textinput_free(&modulizeTI);

	window_free(&mainWindow);
	window_free(&searchWindow);
	search_free(&search);

	TTF_CloseFont(font);
	TTF_CloseFont(searchbarFont);
	TTF_CloseFont(modulizeFont);

	guigfx_free_nslice(&textBoxTexture);
	guigfx_free_nslice(&textBoxTextureModulize);
	guigfx_free_nslice(&panelTexture);

	button_free(&newFileB);
	button_free(&openFileB);
	button_free(&saveFileB);
	button_free(&modulizeB);
	button_free(&addModuleB);
	button_free(&simulateB);
	button_free(&drawB);

	window_quit_SDL();

	nodev_free(&vec);

	return 0;
}