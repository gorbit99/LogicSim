#include <stdio.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdbool.h>
#include "Graphics.h"

int main(int argc, char **argv) {

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window *window = SDL_CreateWindow("Test window",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			640,
			480,
			SDL_WINDOW_SHOWN);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);



	bool quit = false;
	SDL_Event e;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					quit = true;
					break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		int x, y;
		SDL_GetMouseState(&x, &y);

		SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormat(0, 640, 480, 32, SDL_PIXELFORMAT_ABGR32);
		gfx_draw_bezier_cubic(surf, (Point){100, 100}, (Point){600, 400}, (Point){x, y}, (Point){300, 400}, 5, 0x0000ffff);
		SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
		SDL_FreeSurface(surf);
		SDL_RenderCopy(renderer, tex, NULL, NULL);
		SDL_DestroyTexture(tex);

		SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}