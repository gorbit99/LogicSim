#include "GUIGraphics.h"

NSliceTexture guigfx_create_nslice(char *path, SliceType type, int x1, int y1, int x2, int y2, SDL_Renderer *renderer) {
	SDL_Surface *surface = IMG_Load(path);
	int w, h;
	w = surface->w;
	h = surface->h;
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	NSliceTexture result = {
			texture,
			w, h,
			type,
			x1, y1, x2, y2
	};
	return result;
}

void guigfx_render_9slice(NSliceTexture *data, SDL_Rect rect, SDL_Renderer *renderer) {
	SDL_Rect src, dst;

	//Corners
	src = (SDL_Rect){0, 0, data->x1, data->y1};
	dst = (SDL_Rect){rect.x +  src.x, rect.y +  src.y, src.w, src.h};
	SDL_RenderCopy(renderer, data->texture, &src, &dst);
	src = (SDL_Rect){data->x2, 0, data->w - data->x2, data->y1};
	dst = (SDL_Rect){rect.x + rect.w - src.w, rect.y +  src.y, src.w, src.h};
	SDL_RenderCopy(renderer, data->texture, &src, &dst);
	src = (SDL_Rect){0, data->y2, data->x1, data->h - data->y2};
	dst = (SDL_Rect){rect.x +  src.x, rect.y + rect.h - src.h, src.w, src.h};
	SDL_RenderCopy(renderer, data->texture, &src, &dst);
	src = (SDL_Rect){data->x2, data->y2, data->w - data->x2, data->h - data->y2};
	dst = (SDL_Rect){rect.x + rect.w - src.w, rect.y + rect.h - src.h, src.w, src.h};
	SDL_RenderCopy(renderer, data->texture, &src, &dst);

}