#ifndef HOMEWORK_UTILITY_H
#define HOMEWORK_UTILITY_H

/**
 * @file Utility.h
 * @brief Utility functions for other header files
 * @version 0.1
 * @date 2019-11-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <SDL.h>
#include <stdio.h>

/**
 * @brief Log error
 * 
 */
#define log_error(fmt, ...) (log_error_base(__FILE__, __LINE__, fmt, ##__VA_ARGS__))

/**
 * @brief Minimum of two values
 * 
 */
#define min(a, b) ((a) < (b) ? (a) : (b))

/**
 * @brief Maximum of two values
 * 
 */
#define max(a, b) ((a) > (b) ? (a) : (b))

/**
 * @brief Create an rgba surface
 * 
 * @param w Width of the surface
 * @param h Height of the surface
 * @return SDL_Surface* Resulting surface
 */
SDL_Surface *get_rgba_surface(int w, int h);

/**
 * @brief Get the file name from a path
 * 
 * @param path Path to parse
 * @return char* Resulting file name
 */
char *file_from_path(char *path);

/**
 * @brief Base function for logging errors
 * 
 * @param file File where log happens
 * @param line Line where log happens
 * @param fmt Format string
 * @param ... Input
 */
void log_error_base(const char *file, int line, const char *fmt, ...);

#endif //HOMEWORK_UTILITY_H
