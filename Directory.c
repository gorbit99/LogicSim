#include "Directory.h"
#include "debugmalloc.h"

#ifdef __WINDOWS__

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

FileList dir_getfiles(char *path) {
	size_t length;
	StringCchLength(path, MAX_PATH, &length);
	if (length > MAX_PATH - 3)
		return (FileList){NULL, 0};

	TCHAR dir[MAX_PATH];

	StringCchCopy(dir, MAX_PATH, path);
	StringCchCat(dir, MAX_PATH, TEXT("\\*"));

	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile(dir, &findData);


	if (hFind == INVALID_HANDLE_VALUE)
		return (FileList){NULL, 0};

	size_t num = 0;
	char **results = NULL;

	do {
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			num++;
			char **newMem = (char **)realloc(results, sizeof(char *) * num);
			if (newMem == NULL) {
				log_error("Couldn't allocate new memory!\n");
				num--;
				return (FileList){results, num};
			}
			results = newMem;

			size_t len = strlen(findData.cFileName) + 1;
			results[num - 1] = (char *)malloc(sizeof(char) * len);
			StringCchCopy(results[num - 1], len, findData.cFileName);
		}
	} while (FindNextFile(hFind, &findData) != 0);

	return (FileList){results, num};
}
#else

#include <dirent.h>
#include <sys/stat.h>

FileList dir_getfiles(char *path) {
	DIR *dir = opendir(path);

	if (!dir)
		return (FileList){NULL, 0};

	size_t num = 0;
	char **results = NULL;

	size_t pathLen = strlen(path);

	for (struct dirent *ent = readdir(dir); ent != NULL; ent = readdir(dir)) {

		char *fullPath = (char *)malloc(sizeof(char) * (pathLen + strlen(ent->d_name) + 2));
		strcpy(fullPath, path);
		strcat(fullPath, "/");
		strcat(fullPath, ent->d_name);

		struct stat fileStat;
		stat(fullPath, &fileStat);
		free(fullPath);

		if (S_ISREG(fileStat.st_mode)) {
			num++;
			char **newMem = (char **)realloc(results, sizeof(char *) * num);
			if (newMem == NULL) {
				log_error("Couldn't allocate new memory!\n");
				num--;
				return (FileList){results, num};
			}
			results = newMem;

			size_t len = strlen(ent->d_name) + 1;
			results[num - 1] = (char *)malloc(sizeof(char) * len);
			strcpy(results[num - 1], ent->d_name);
		}
	}

	closedir(dir);

	return (FileList) {results, num};
}

#endif

void dir_free_filelist(FileList *fileList) {
	for (size_t i = 0; i < fileList->num; i++) {
		free((fileList->files)[i]);
	}
	free(fileList->files);
}

void dir_remove_extension(char *file) {
	*dir_get_extension(file) = '\0';
}

char *dir_get_extension(char *file) {
    size_t len = strlen(file);
    for (size_t i = len; i > 0; i--) {
        if (file[i] == '.') {
            return &file[i];
        }
    }
}
