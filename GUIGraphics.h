#ifndef HOMEWORK_GUIGRAPHICS_H
#define HOMEWORK_GUIGRAPHICS_H

/**
 * @file GUIGraphics.h
 * @brief Structures and functions related to GUI Graphics
 * @version 0.1
 * @date 2019-11-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include "Utility.h"

/**
 * @brief Type of a 9-slice texture
 * 
 */
typedef enum SliceType {
	ST_CLAMP, /**< Clamp the texture to the edges */
	ST_REPEAT /**< Repeat texture over the given area */
} SliceType;

/**
 * @brief Data for a 9-slice texture
 * 
 */
typedef struct NSliceTexture {
	SDL_Texture *texture; /**< Texture */
	int w, /**< Width */ 
		h; /**< Height */
	SliceType type; /**< Type of the texture */
	int x1, /**< Relative position of the first vertical slice */
		y1, /**< Relative position of the first horizontal slice */
		x2, /**< Relative position of the second vertical slice */
		y2; /**< Relative position of the second horizontal slice */
	int w1, /**< Width of the first inner slice */
		w2, /**< Width of the second inner slice */
		w3; /**< Width of the third inner slice */
	int h1, /**< Height of the first inner slice */
		h2, /**< Height of the second inner slice */
		h3; /**< Height of the third inner slice */
} NSliceTexture;

/**
 * @brief Create 9-slice texture
 * 
 * @param path Path to load image from
 * @param type Type of the texture
 * @param x1 First vertical slice x position
 * @param y1 First horizontal slice y position
 * @param x2 Second vertical slice x position
 * @param y2 Second horizontal slice y position
 * @param renderer Renderer to send texture to
 * @return NSliceTexture Resulting 9-slice texture
 */
NSliceTexture guigfx_create_nslice(char *path, SliceType type, int x1, int y1, int x2, int y2, SDL_Renderer *renderer);

/**
 * @brief Render 9-slice texture into given area
 * 
 * @param data Texture to render
 * @param rect Rect to render to
 * @param renderer Renderer to render with
 */
void guigfx_render_nslice(NSliceTexture *data, SDL_Rect rect, SDL_Renderer *renderer);

/**
 * @brief Frees a 9-slice texture
 *
 * @param data Texture to free
 */

void guigfx_free_nslice(NSliceTexture *data);

#endif //HOMEWORK_GUIGRAPHICS_H
