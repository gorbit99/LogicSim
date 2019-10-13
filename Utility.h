#ifndef HOMEWORK_UTILITY_H
#define HOMEWORK_UTILITY_H

#include <SDL.h>

SDL_Surface *get_rgba_surface(int w, int h);
size_t read_until_delim(const char *str, char *result, size_t *startIndex, char delim);

#endif //HOMEWORK_UTILITY_H
