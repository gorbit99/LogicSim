#ifndef HOMEWORK_UTILITY_H
#define HOMEWORK_UTILITY_H

#include <SDL.h>
#include <stdio.h>

#define log_error(fmt, ...) (log_error_base(__FILE__, __LINE__, fmt, ##__VA_ARGS__))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

SDL_Surface *get_rgba_surface(int w, int h);
char *file_from_path(char *path);

void log_error_base(const char *file, int line, const char *fmt, ...);

#endif //HOMEWORK_UTILITY_H
