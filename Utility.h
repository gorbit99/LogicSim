#ifndef HOMEWORK_UTILITY_H
#define HOMEWORK_UTILITY_H

#include <SDL.h>
#include <stdio.h>

#define log_error(fmt, ...) (log_error_base(__FILE__, __LINE__, fmt, ##__VA_ARGS__))

SDL_Surface *get_rgba_surface(int w, int h);
size_t read_until_delim(const char *str, char *result, size_t *startIndex, char delim);
char *file_from_path(char *path);

void log_error_base(const char *file, int line, const char *fmt, ...);

#endif //HOMEWORK_UTILITY_H
