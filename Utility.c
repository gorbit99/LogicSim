#include "Utility.h"
#include "debugmalloc.h"

SDL_Surface *get_rgba_surface(int w, int h) {
	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ABGR32);
	return surface;
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
	va_start(valist, fmt);
	SDL_LogMessageV(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_CRITICAL, fmt, valist);
	va_end(valist);
}

SDL_Texture *load_texture(const char *path, SDL_Renderer *renderer) {
	SDL_Surface *surf = IMG_Load(path);
	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
	return tex;
}

bool strcmp_nocase(char *A, char *B) {
	int i;
	for (i = 0; A[i] != '\0'; i++) {
		if (tolower(A[i]) != tolower(B[i]))
			return false;
	}
	return B[i] == '\0';
}
