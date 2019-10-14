#include <stdio.h>
#define SDL_MAIN_HANDLED

#include "debugmalloc.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include "Graphics.h"
#include "Component.h"

int main(int argc, char **argv) {

	//debugmalloc_log_file("log.txt");

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	SDL_Window *window = SDL_CreateWindow("Test window",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			640,
			480,
			SDL_WINDOW_SHOWN);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	TTF_Font *font = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 50);
	SDL_Texture *testT;
	SDL_Surface *test = component_load_graphic("res/XOR.cmp", 200, 5, font);
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
						SDL_Surface *test = component_load_graphic("res/BUF.cmp", 200, 5, font);
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
		SDL_Rect r = {0, 0, 200, 200};
		SDL_RenderCopy(renderer, testT, NULL, &r);

		SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();

	malloc(sizeof(int) * 20);

	return 0;
}