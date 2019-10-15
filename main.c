#include <stdio.h>
#define SDL_MAIN_HANDLED

#include "debugmalloc.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include "Graphics.h"
#include "Component.h"
#include "Window.h"

int main(int argc, char **argv) {

	debugmalloc_log_file("debugmalloclog.txt");

	SDL_Window *window;
	SDL_Renderer *renderer;

	window_init(&window, &renderer, "Logic Simulator", 640, 480, SDL_WINDOW_SHOWN, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	TTF_Init();

	TTF_Font *font = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 50);
	SDL_Texture *testT;
	SDL_Surface *test = component_load_graphic("res/XNOR.cmp", 200, 5, font);
	testT = SDL_CreateTextureFromSurface(renderer, test);
	SDL_FreeSurface(test);

	bool quit = false;
	SDL_Event e;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN: {
					if (e.key.keysym.sym == SDLK_SPACE) {
						SDL_DestroyTexture(testT);
						test = component_load_graphic("res/BUF.cmp", 200, 5, font);
						testT = SDL_CreateTextureFromSurface(renderer, test);
						SDL_FreeSurface(test);
					}
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		int x, y;
		SDL_GetMouseState(&x, &y);
		int w, h;
		SDL_QueryTexture(testT, NULL, NULL, &w, &h);
		SDL_Rect r = {x, y, w, h};
		SDL_RenderDrawLine(renderer, 0, 0, 639, 479);
		SDL_RenderCopy(renderer, testT, NULL, &r);

		SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();

	return 0;
}