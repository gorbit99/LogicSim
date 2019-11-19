#include "Component.h"

#include "debugmalloc.h"

const float COMPSIZE = 300;
const float COMPTHICKNESS = 15;

SDL_Surface *component_load_graphic(const char *path, float size, float thickness, TTF_Font *font) {

	FILE *f = fopen(path, "r");

	float wf, hf;
	fscanf(f, "%f %f", &wf, &hf);

	int w = (int) ceilf(wf * size), h = (int) ceilf(hf * size);

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
		} else if (strcmp(identifier, "C") == 0) {
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
		} else if (strcmp(identifier, "QB") == 0) {
			Point V1, V2, C;
			fscanf(f, "%f,%f %f,%f %f,%f", &V1.x, &V1.y, &V2.x, &V2.y, &C.x, &C.y);
			V1.x *= size;
			V1.y *= size;
			V2.x *= size;
			V2.y *= size;
			C.x *= size;
			C.y *= size;
			gfx_draw_bezier(component, V1, V2, C, thickness, 0xffffffff);
		} else if (strcmp(identifier, "QBC") == 0) {
			Point V1, V2, C1, C2;
			fscanf(f, "%f,%f %f,%f %f,%f %f,%f", &V1.x, &V1.y, &V2.x, &V2.y, &C1.x, &C1.y, &C2.x, &C2.y);
			V1.x *= size;
			V1.y *= size;
			V2.x *= size;
			V2.y *= size;
			C1.x *= size;
			C1.y *= size;
			C2.x *= size;
			C2.y *= size;
			gfx_draw_bezier_cubic(component, V1, V2, C1, C2, thickness, 0xffffffff);
		} else if (strcmp(identifier, "TXT") == 0) {
			float xf, yf;
			char txt[17];
			fscanf(f, "%f,%f %s", &xf, &yf, txt);
			int x = (int) (xf * size), y = (int) (yf * size);
			if (font != NULL) {
				SDL_Surface *text = TTF_RenderText_Blended(font, txt, (SDL_Color) {255, 255, 255, 255});
				SDL_Rect blitRect = {x - text->w / 2, y - text->h / 2, text->w, text->h};
				SDL_BlitSurface(text, NULL, component, &blitRect);
			}
		} else if (strcmp(identifier, "AABB") == 0) {
			Point pos, s;
			fscanf(f, "%f,%f %f,%f", &pos.x, &pos.y, &s.x, &s.y);
			pos.x *= size;
			pos.y *= size;
			s.x *= size;
			s.y *= size;
			gfx_fill_triangle(component, pos, (Point){pos.x + s.x, pos.y}, (Point){pos.x, pos.y + s.y}, 0xffffffff);
			gfx_fill_triangle(component, (Point){pos.x + s.x, pos.y}, (Point){pos.x, pos.y + s.y}, (Point){pos.x + s.x, pos.y + s.y}, 0xffffffff);
		}
	}

	fclose(f);

	return component;
}

ComponentData component_create(float x, float y, char *name, float size, float thickness,
							   TTF_Font *font, SDL_Renderer *renderer) {
	ComponentData data;
	data.x = x;
	data.y = y;
	char path[256];
	sprintf(path, "res/Modules/%s.cmp", name);
	SDL_Surface *surf = component_load_graphic(path, size, thickness, font);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
	data.texture = texture;
	data.w = surf->w;
	data.h = surf->h;
	SDL_FreeSurface(surf);

	sprintf(path, "res/Modules/%s.dat", name);
	data.pinData = component_load_pin_data(path, size);

	sprintf(path, "res/Modules/%s.fun", name);
	data.funData = parser_load_function(path);

	data.type = CT_MODULE;

	return data;
}

void component_free_data(ComponentData *dat) {
	free(dat->pinData.pins);
	parser_free_function(&dat->funData);
}

void component_render(ComponentData *dat, SDL_Renderer *renderer, Point camPos, Color color) {
	SDL_FRect r = {
			(dat->x - camPos.x - (float)dat->w / 2),
			(dat->y - camPos.y - (float)dat->h / 2),
			(float) dat->w,
			(float) dat->h
	};

	SDL_SetTextureColorMod(dat->texture, (color & 0xff000000u) >> 24u, (color & 0x00ff0000u) >> 16u, (color & 0x0000ff00u) >> 8u);
	SDL_RenderCopyF(renderer, dat->texture, NULL, &r);
}

SDL_Surface *component_create_wire_texture(Point V1, Point V2, float ang1, float ang2, float size, float thickness, Point *pin1Pos) {
	float dx1 = cosf(ang1) * size / 2;
	float dy1 = sinf(ang1) * size / 2;
	float dx2 = cosf(ang2) * size / 2;
	float dy2 = sinf(ang2) * size / 2;

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

	SDL_Surface *wire = get_rgba_surface((int) (3 * thickness + maxX - minX), (int) (3 * thickness + maxY - minY));
	V1.x += 1.5f * thickness - minX;
	V1.y += 1.5f * thickness - minY;
	V2.x += 1.5f * thickness - minX;
	V2.y += 1.5f * thickness - minY;
	C1.x += 1.5f * thickness - minX;
	C1.y += 1.5f * thickness - minY;
	C2.x += 1.5f * thickness - minX;
	C2.y += 1.5f * thickness - minY;

	*pin1Pos = V1;

	gfx_draw_bezier_cubic(wire, V1, V2, C1, C2, thickness, 0xffffffff);
	return wire;
}

PinData component_load_pin_data(const char *path, float size) {
	PinData data = {};
	FILE *f = fopen(path, "r");
	if (f == NULL) {
		log_error("Couldn't open file: %s!", path);
		return data;
	}
	fscanf(f, "%d", &data.pinCount);
	data.pins = malloc(sizeof(Pin) * data.pinCount);

	for (int i = 0; i < data.pinCount; i++) {
		char type[10];
		fscanf(f, "%s %s %f,%f %f", type, data.pins[i].name, &data.pins[i].pos.x,
			   &data.pins[i].pos.y, &data.pins[i].angle);
		data.pins[i].pos.x *= size;
		data.pins[i].pos.y *= size;
		if (strcmp(type, "in") == 0) data.pins[i].type = PIN_IN;
		if (strcmp(type, "out") == 0) data.pins[i].type = PIN_OUT;
		data.pins[i].occupied = false;
	}
	fclose(f);
	return data;
}

ComponentData component_create_wire_between(ComponentData *comp1, ComponentData *comp2, int pin1, int pin2,
		float size, float thickness, SDL_Renderer *renderer) {
	ComponentData data;

	Point pin1Pos;

	Point pinPos1 = component_get_pin_position(comp1, pin1);
	Point pinPos2 = component_get_pin_position(comp2, pin2);
	SDL_Surface *surf = component_create_wire_texture(
		pinPos1,
		pinPos2,
		comp1->pinData.pins[pin1].angle, 
		comp2->pinData.pins[pin2].angle, 
		size, 
		thickness,
		&pin1Pos);

	data.texture = SDL_CreateTextureFromSurface(renderer, surf);
	data.w = surf->w;
	data.h = surf->h;
	SDL_FreeSurface(surf);
	data.x = pinPos1.x - pin1Pos.x + (float)data.w / 2;
	data.y = pinPos1.y - pin1Pos.y + (float)data.h / 2;

	data.pinData.pinCount = 2;
	data.pinData.pins = NULL;

	data.funData.assignC = 0;
	data.funData.assigns = NULL;
	data.funData.inC = 0;
	data.funData.outC = 0;
	data.funData.wires = NULL;

	data.type = CT_WIRE;

	return data;
}

ComponentData component_create_LED(float x, float y, float size, float thickness, SDL_Renderer *renderer) {
	ComponentData data = component_create(x, y, "LED", size, thickness, NULL, renderer);
	data.type = CT_LED;
	return data;
}

ComponentData component_create_switch(float x, float y, float size, float thickness, SDL_Renderer *renderer) {
	ComponentData data = component_create(x, y, "SWITCH", size, thickness, NULL, renderer);
	data.type = CT_SWITCH;
	return data;
}

void component_run(ComponentData *component, bool *in, bool *out) {
	parser_run_function(&component->funData, in, out);
}

Point component_get_pin_position(ComponentData *componentData, int pin) {
	Point result = {componentData->x + componentData->pinData.pins[pin].pos.x - (float)componentData->w / 2,
	                componentData->y + componentData->pinData.pins[pin].pos.y - (float)componentData->h / 2};
	return result;
}
