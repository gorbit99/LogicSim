#include "Utility.h"

#include "debugmalloc.h"

SDL_Surface *get_rgba_surface(int w, int h) {
	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ABGR32);
	return surface;
}

size_t read_until_delim(const char *str, char *result, size_t *startIndex, char delim) {
	size_t pos = 0;
	while(str[*startIndex + pos] != delim && str[*startIndex + pos] != '\0') {
		result[pos] = str[*startIndex + pos];
		pos++;
	}
	result[pos] = '\0';

	if (str[*startIndex + pos] == '\0') *startIndex = pos;
	else *startIndex += pos + 1;
	return 0;
}

char *file_from_path(char *path) {
	size_t last = 0;
	for (int i = 0; path[i] != '\0';  i++) {
		if (path[i] == '\\' || path[i] == '/')
			last = i;
	}
	return path + last + 1;
}

void log_error_base(const char *file, int line, const char *fmt, ...) {
	va_list valist;
	char *fileName = file_from_path((char *)file);
	SDL_LogMessage(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_CRITICAL, "Error in file %s on line %d:\n", fileName, line);
	va_start(valist, fmt);
	SDL_LogMessageV(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_CRITICAL, fmt, valist);
	va_end(valist);
}
