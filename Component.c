#include "Component.h"

SDL_Surface *component_load_graphic(const char *path) {

	FILE *f = fopen(path, "r");

	int w, h;
	fscanf(f, "%d %d", &w, &h);

	SDL_Surface *component = get_rgba_surface(w, h);


	char identifier[10];
	while (fscanf(f, "%s", identifier) == 1) {

		if (strcmp(identifier, "L") == 0) {
			Point V1, V2;
			fscanf(f, "%f,%f %f,%f", &V1.x, &V1.y, &V2.x, &V2.y);
			V1.x *= 100;
			V1.y *= 100;
			V2.x *= 100;
			V2.y *= 100;
			gfx_draw_thick_line(component, V1, V2, 2, 0xffffffff);
		}
		if (strcmp(identifier, "C") == 0) {
			Point C;
			float r;
			char fill;
			fscanf(f, "%f,%f %f %c", &C.x, &C.y, &r, &fill);
			C.x *= 100;
			C.y *= 100;
			r *= 100;
			if (fill == 'f')
				gfx_fill_circle(component, C, r, 0xffffffff);
			else if (fill == 'e')
				gfx_draw_circle(component, C, r, 0xffffffff);
		}
		if (strcmp(identifier, "QB") == 0) {
			Point V1, V2, C;
			fscanf(f, "%f,%f %f,%f %f,%f", &V1.x, &V1.y, &V2.x, &V2.y, &C.x, &C.y);
			V1.x *= 100;
			V1.y *= 100;
			V2.x *= 100;
			V2.y *= 100;
			C.x *= 100;
			C.y *= 100;
			gfx_draw_bezier(component, V1, V2, C, 2, 0xffffffff);
		}
	}

	return component;
}
