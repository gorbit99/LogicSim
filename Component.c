#include "Component.h"

#include "debugmalloc.h"

SDL_Surface *component_load_graphic(const char *path, float size, float thickness, TTF_Font *font) {

	FILE *f = fopen(path, "r");

	float wf, hf;
	fscanf(f, "%f %f", &wf, &hf);

	int w = (int)ceilf(wf * size), h = (int)ceilf(hf * size);

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
			else if (fill == 'e') {
				gfx_fill_ring(component, C, r, thickness, 0xffffffff);
			}
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

	fclose(f);

	return component;
}

ComponentData component_create_data(float x, float y, SDL_Texture *texture, size_t outCount) {
	ComponentData dat = {
		x,
		y,
		0,
		0,
		texture,
		outCount,
		(bool *)calloc(outCount, sizeof(bool))
	};
	SDL_QueryTexture(texture, NULL, NULL, &dat.w, &dat.h);
	return dat;
}

void component_update_texture(ComponentData *dat, SDL_Texture *texture) {
	SDL_QueryTexture(texture, NULL, NULL, &(dat->w), &(dat->h));
	dat->texture = texture;
}

void component_free_data(ComponentData *dat) {
	free(dat->out);
}

void component_render(ComponentData *dat, SDL_Renderer *renderer, Point camPos) {
	SDL_Rect r = {
		(int)(dat->x - camPos.x),
		(int)(dat->y - camPos.y),
		dat->w,
		dat->h
	};
	SDL_RenderCopy(renderer, dat->texture, NULL, &r);
}

SDL_Surface *component_create_wire(Point V1, Point V2, float ang1, float ang2, float size, float thickness) {
	float dx1 = cosf(ang1) * size / 5;
	float dy1 = sinf(ang1) * size / 5;
	float dx2 = cosf(ang2) * size / 5;
	float dy2 = sinf(ang2) * size / 5;

	Point C1 = {V1.x + dx1, V1.y + dy1};
	Point C2 = {V2.x + dx2, V2.y + dy2};
	float minX = V1.x;
	if (V2.x < minX) minX = V2.x;
	if (C1.x < minX) minX = C1.x;
	if (C2.x < minX) minX = C2.x;
	float maxX = V1.x;
	if (V2.x > maxX) maxX = V2.x;
	if (C1.x > maxX) maxX = C1.x;
	if (C2.x > maxX) maxX = C2.x;
	float minY = V1.y;
	if (V2.y < minY) minY = V2.y;
	if (C1.y < minY) minY = C1.y;
	if (C2.y < minY) minY = C2.y;
	float maxY = V1.y;
	if (V2.y > maxY) maxY = V2.y;
	if (C1.y > maxY) maxY = C1.y;
	if (C2.y > maxY) maxY = C2.y;

	SDL_Surface *wire = get_rgba_surface((int)(3 * thickness + maxX - minX), (int)(3 * thickness + maxY - minY));
	V1.x += 1.5f * thickness - minX;
	V1.y += 1.5f * thickness - minY;
	V2.x += 1.5f * thickness - minX;
	V2.y += 1.5f * thickness - minY;
	C1.x += 1.5f * thickness - minX;
	C1.y += 1.5f * thickness - minY;
	C2.x += 1.5f * thickness - minX;
	C2.y += 1.5f * thickness - minY;

	gfx_draw_bezier_cubic(wire, V1, V2, C1, C2, thickness, 0xffffffff);
	return wire;
}