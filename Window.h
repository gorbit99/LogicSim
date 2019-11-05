#ifndef HOMEWORK_WINDOW_H
#define HOMEWORK_WINDOW_H

/**
 * @file Window.h
 * @brief Functions handling SDL 2
 * @version 0.1
 * @date 2019-11-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <time.h>
#include "Utility.h"

/**
 * @brief Initialize SDL Window and Renderer 
 * 
 * @param window Pointer to the window to initialize
 * @param renderer Pointer to the renderer to initialize
 * @param title Title of the window
 * @param w Width of the window
 * @param h Height of the window
 * @param windowFlags Flags for the window
 * @param rendererFlags Flags for the renderer
 * @return true Successful initialization
 * @return false There was an error during initialization
 */
bool window_init(SDL_Window **window, SDL_Renderer **renderer, const char *title, int w, int h, int windowFlags, int rendererFlags);

/**
 * @brief Cleanup window
 * 
 * @param window Window to cleanup
 * @param renderer Renderer to cleanup
 */
void window_cleanup(SDL_Window *window, SDL_Renderer *renderer);

#endif //HOMEWORK_WINDOW_H
