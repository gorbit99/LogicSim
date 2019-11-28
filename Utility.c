#include "Utility.h"
#include "debugmalloc.h"

SDL_Surface *get_rgba_surface(int w, int h) {
	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ABGR32);
	return surface;
}

void log_error_base(const char *file, int line, const char *fmt, ...) {
	va_list valist;
	va_start(valist, fmt);
	FILE *log = fopen("log.txt", "a");
	if (log == NULL)
		return;
	fprintf(log, "Error in file %s on line %d:\n", file, line);
	vfprintf(log, fmt, valist);
	va_end(valist);
	fclose(log);
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
