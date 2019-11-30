#ifndef HOMEWORK_FILEDIALOG_H
#define HOMEWORK_FILEDIALOG_H

#include <SDL.h>
#include <SDL_platform.h>
#ifdef __WINDOWS__
#include <windows.h>
#endif
#ifdef __LINUX__
#include <gtk-3.0/gtk/gtk.h>
#endif
#include <SDL_syswm.h>
#include "Directory.h"

typedef enum DialogType {
	DT_SAVE,
	DT_OPEN
} DialogType;

char *open_file_dialog(SDL_Window *owner, const char *filter, const char *title, DialogType type);

#endif //HOMEWORK_FILEDIALOG_H
