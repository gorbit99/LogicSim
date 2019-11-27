#include "GUIGraphics.h"
#include "debugmalloc.h"

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
			x1, y1, x2, y2,
			x1, x2 - x1, w - x2,
			y1, y2 - y1, h - y2
	};
	return result;
}

void guigfx_render_nslice(NSliceTexture *data, SDL_Rect rect, SDL_Renderer *renderer) {
	SDL_Rect src, dst;

	int x1, x2, x3;
	int y1, y2, y3;
	int w1, w2, w3;
	int h1, h2, h3;

	x1 = rect.x; x2 = rect.x + data->w1; x3 = rect.x + rect.w - data->w3;
	y1 = rect.y; y2 = rect.y + data->h1; y3 = rect.y + rect.h - data->h3;
	w1 = x2 - x1; w2 = x3 - x2; w3 = x1 + rect.w - x3; 
	h1 = y2 - y1; h2 = y3 - y2; h3 = y1 + rect.h - y3;

	//Corners
	src = (SDL_Rect){0, 0, data->w1, data->h1};
	dst = (SDL_Rect){x1, y1, w1, h1};
	SDL_RenderCopy(renderer, data->texture, &src, &dst);
	src = (SDL_Rect){data->x2, 0, data->w3, data->h1};
	dst = (SDL_Rect){x3, y1, w3, h1};
	SDL_RenderCopy(renderer, data->texture, &src, &dst);
	src = (SDL_Rect){0, data->y2, data->w1, data->h3};
	dst = (SDL_Rect){x1, y3, w1, h3};
	SDL_RenderCopy(renderer, data->texture, &src, &dst);
	src = (SDL_Rect){data->x2, data->y2, data->w3, data->h3};
	dst = (SDL_Rect){x3, y3, w3, h3};
	SDL_RenderCopy(renderer, data->texture, &src, &dst);

	switch (data->type)
	{
	case ST_CLAMP: {
		//Top
		src = (SDL_Rect){data->x1, 0, data->w2, data->h1};
		dst = (SDL_Rect){x2, y1, w2, h1};
		SDL_RenderCopy(renderer, data->texture, &src, &dst);
		//Bottom
		src = (SDL_Rect){data->x1, data->y2, data->w2, data->h3};
		dst = (SDL_Rect){x2, y3, w2, h3};
		SDL_RenderCopy(renderer, data->texture, &src, &dst);
		//Left
		src = (SDL_Rect){0, data->y1, data->w1, data->h2};
		dst = (SDL_Rect){x1, y2, w1, h2};
		SDL_RenderCopy(renderer, data->texture, &src, &dst);
		//Right
		src = (SDL_Rect){data->x2, data->y1, data->w3, data->h2};
		dst = (SDL_Rect){x3, y2, w3, h2};
		SDL_RenderCopy(renderer, data->texture, &src, &dst);
		//Center
		src = (SDL_Rect){data->x1, data->y1, data->w2, data->h2};
		dst = (SDL_Rect){x2, y2, w2, h2};
		SDL_RenderCopy(renderer, data->texture, &src, &dst);
		break;
	}
	case ST_REPEAT: {
		//Top
		SDL_Rect clipRect = {x2, y1, w2, rect.h};
		SDL_RenderSetClipRect(renderer, &clipRect);
		src = (SDL_Rect){data->x1, 0, data->w2, data->h1};
		int x;
		for (x = x2; x < x3; x += data->w2) {
			dst = (SDL_Rect){x, y1, data->w2, h1};
			SDL_RenderCopy(renderer, data->texture, &src, &dst);
		}
		//Bottom
		src = (SDL_Rect){data->x1, data->x2, data->w2, data->h3};
		for (x = x2; x < x3; x += data->w2) {
			dst = (SDL_Rect){x, y3, data->w2, h3};
			SDL_RenderCopy(renderer, data->texture, &src, &dst);
		}
		//Left
		clipRect = (SDL_Rect){x1, y2, rect.w, h2};
		SDL_RenderSetClipRect(renderer, &clipRect);
		src = (SDL_Rect){0, data->y1, data->w1, data->h2};
		int y;
		for (y = y2; y < y3; y += data->h2) {
			dst = (SDL_Rect){x1, y, w1, data->h2};
			SDL_RenderCopy(renderer, data->texture, &src, &dst);
		}
		//Right
		src = (SDL_Rect){data->x2, data->y1, data->w3, data->h2};
		for (y = y2; y < y3; y += data->h2) {
			dst = (SDL_Rect){x3, y, w3, data->h2};
			SDL_RenderCopy(renderer, data->texture, &src, &dst);
		}
		//Center
		clipRect = (SDL_Rect){x2, y2, w2, h2};
		SDL_RenderSetClipRect(renderer, &clipRect);
		src = (SDL_Rect){data->x1, data->y1, data->w2, data->h2};
		for (x = x2; x < x3; x += data->w2) {
			for (y = y2; y < y3; y += data->h2) {
				dst = (SDL_Rect){x, y, data->w2, data->h2};
				SDL_RenderCopy(renderer, data->texture, &src, &dst);
			}
		}
		SDL_RenderSetClipRect(renderer, NULL);
		break;
	}
	default:
		break;
	}
}

void guigfx_free_nslice(NSliceTexture *data) {
	SDL_DestroyTexture(data->texture);
}
