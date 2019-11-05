#ifndef HOMEWORK_GRAPHICS_H
#define HOMEWORK_GRAPHICS_H

/**
 * @file Graphics.h
 * @brief Structures and functions related to drawing shapes
 * @version 0.1
 * @date 2019-11-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "debugmalloc.h"

#include <stddef.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <assert.h>

/**
 * @brief Represents a point
 * 
 */
typedef struct Point {
	float x, /**< X Position */
		  y; /**< Y Position */
} Point;

/**
 * @brief Represents a vector
 * 
 */
typedef Point Vec;

/**
 * @brief Represents a color
 * 
 */
typedef uint32_t Color;

/**
 * @brief Draw one pixel line between two points
 * 
 * @param dest Surface to draw to
 * @param V1 First point
 * @param V2 Second point
 * @param color Color to use
 */
void gfx_draw_line(SDL_Surface *dest, Point V1, Point V2, Color color);

/**
 * @brief Draw a one pixel outline around the given triangle
 * 
 * @param dest Surface to draw to
 * @param V1 First point
 * @param V2 Second point
 * @param V3 Third point
 * @param color Color to use
 */
void gfx_draw_triangle(SDL_Surface *dest, Point V1, Point V2, Point V3, Color color);

/**
 * @brief Fill the given triangle
 * 
 * @param dest Surface to draw to
 * @param V1 First point
 * @param V2 Second point
 * @param V3 Third point
 * @param color Color to use
 */
void gfx_fill_triangle(SDL_Surface *dest, Point V1, Point V2, Point V3, Color color);

/**
 * @brief Draw a line with thickness
 * 
 * @param dest Surface to draw to
 * @param V1 First point
 * @param V2 Second point
 * @param thickness Thickness of the line
 * @param color Color to use
 */
void gfx_draw_thick_line(SDL_Surface *dest, Point V1, Point V2, float thickness, Color color);

/**
 * @brief Draw one pixel circle around point with radius
 * 
 * @param dest Surface to draw to
 * @param C Center position
 * @param r Radius
 * @param color Color to use
 */
void gfx_draw_circle(SDL_Surface *dest, Point C, float r, Color color);

/**
 * @brief Fill circle with center and radius
 * 
 * @param dest Surface to draw to
 * @param C Center position
 * @param r Radius
 * @param color Color to use
 */
void gfx_fill_circle(SDL_Surface *dest, Point C, float r, Color color);

/**
 * @brief Draw quadratic bezier curve with given information
 * 
 * @param dest Surface to draw to
 * @param V1 First endpoint
 * @param V2 Second endpoint
 * @param C Control point
 * @param thickness Thickness of curve
 * @param color Color to use
 */
void gfx_draw_bezier(SDL_Surface *dest, Point V1, Point V2, Point C, float thickness, Color color);

/**
 * @brief Draw a cubic bezier curve with given information
 * 
 * @param dest Surface to draw to
 * @param V1 First endpoint
 * @param V2 Second enpoint
 * @param C1 First control point
 * @param C2 Second control point
 * @param thickness Thickness of the curve
 * @param color Color to use
 */
void gfx_draw_bezier_cubic(SDL_Surface *dest, Point V1, Point V2, Point C1, Point C2, float thickness, Color color);

/**
 * @brief Draw text at given position
 * 
 * @param dest Surface to draw to
 * @param font Font to use
 * @param x Top left X
 * @param y Top left Y
 * @param str String to draw
 * @param color Color to use
 */
void gfx_draw_text(SDL_Surface *dest, TTF_Font *font, int x, int y, const char *str, Color color);

/**
 * @brief Fill ring around given point with radius and thickness
 * 
 * @param dest Surface to draw to
 * @param C Center position
 * @param r Radius
 * @param thickness Thickness of the curve
 * @param color Color to use
 */
void gfx_fill_ring(SDL_Surface *dest, Point C, float r, float thickness, Color color);

#endif //HOMEWORK_GRAPHICS_H
