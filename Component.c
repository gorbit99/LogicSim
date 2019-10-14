#include "Component.h"

SDL_Surface *component_load_graphic(const char *path, float size, float thickness, TTF_Font *font) {

	FILE *f = fopen(path, "r");

	float wf, hf;
	fscanf(f, "%f %f", &wf, &hf);

	int w = (int)ceil(wf * size), h = (int)ceil(hf * size);

	SDL_Surface *component = get_rgba_surface(w, h);


	char identifier[10];
	while (fscanf(f, "%s", identifier) == 1) {

		if (strcmp(identifier, "L") == 0) {
			Point V1, V2;
			fscanf(f, "%f,%f %f,%f", &V1.x, &V1.y, &V2.x, &V2.y);
			V1.x *= size;
			V1.y *= size;
			V2.x *= size;
			V2.y *= size;
			gfx_draw_thick_line(component, V1, V2, thickness, 0xffffffff);
		}
		else if (strcmp(identifier, "C") == 0) {
			Point C;
			float r;
			char fill;
			fscanf(f, "%f,%f %f %c", &C.x, &C.y, &r, &fill);
			C.x *= size;
			C.y *= size;
			r *= size;
			if (fill == 'f')
				gfx_fill_circle(component, C, r, 0xffffffff);
			else if (fill == 'e')
				gfx_draw_circle(component, C, r, 0xffffffff);
		}
		else if (strcmp(identifier, "QB") == 0) {
			Point V1, V2, C;
			fscanf(f, "%f,%f %f,%f %f,%f", &V1.x, &V1.y, &V2.x, &V2.y, &C.x, &C.y);
			V1.x *= size;
			V1.y *= size;
			V2.x *= size;
			V2.y *= size;
			C.x *= size;
			C.y *= size;
			gfx_draw_bezier(component, V1, V2, C, thickness, 0xffffffff);
		}
		else if (strcmp(identifier, "TXT") == 0) {
			float xf, yf;
			char txt[17];
			fscanf(f, "%f,%f %s", &xf, &yf, txt);
			int x = (int)(xf * size), y = (int)(yf * size);
			if (font != NULL) {
				SDL_Surface *text = TTF_RenderText_Blended(font, txt, (SDL_Color){255, 255, 255, 255});
				SDL_Rect blitRect = {x - text->w / 2, y - text->h / 2, text->w, text->h};
				SDL_BlitSurface(text, NULL, component, &blitRect);
			}
		}
	}

	return component;
}
