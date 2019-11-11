#ifndef HOMEWORK_DIRECTORY_H
#define HOMEWORK_DIRECTORY_H

#include <SDL_platform.h>
#include <stddef.h>
#include "Utility.h"
#include <string.h>

typedef struct FileList {
	char **files;
	size_t num;
} FileList;

FileList dir_getfiles(char *path);

void dir_free_filelist(FileList *fileList);

void dir_remove_extension(char *file);

#endif //HOMEWORK_DIRECTORY_H
