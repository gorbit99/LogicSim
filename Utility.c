//
// Created by iker on 2019.10.13..
//

#include "Utility.h"

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
