#ifndef HOMEWORK_GRAPHICS_H
#define HOMEWORK_GRAPHICS_H

#include <stddef.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>
#include <assert.h>

#define EPSILON 0.01f

typedef struct Point {
	float x, y;
} Point;

typedef Point Vec;
typedef uint32_t Color;

void gfx_draw_line(SDL_Surface *dest, Point V1, Point V2, Color color);

void gfx_draw_triangle(SDL_Surface *dest, Point V1, Point V2, Point V3, Color color);
void gfx_fill_triangle(SDL_Surface *dest, Point V1, Point V2, Point V3, Color color);

void gfx_draw_circle(SDL_Surface *dest, Point C, float r, Color color);
void gfx_fill_circle(SDL_Surface *dest, Point C, float r, Color color);

void gfx_draw_bezier(SDL_Surface *dest, Point V1, Point V2, Point C, float thickness, Color color);
void gfx_draw_bezier_cubic(SDL_Surface *dest, Point V1, Point V2, Point C1, Point C2, float thickness, Color color);

#endif //HOMEWORK_GRAPHICS_H
