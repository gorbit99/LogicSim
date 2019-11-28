#include "FileDialog.h"
#include "debugmalloc.h"

#ifdef __WINDOWS__

char *open_file_dialog(SDL_Window *owner, const char *filter, const char *title, DialogType type) {
	char file[MAX_PATH];

	SDL_SysWMinfo wMinfo;
	SDL_VERSION(&wMinfo.version);
	if (!SDL_GetWindowWMInfo(owner, &wMinfo)) {
		const char *error = SDL_GetError();
		error = SDL_GetError();
	}
	OPENFILENAMEA ofn;
	ZeroMemory(&file, sizeof(file));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = wMinfo.info.win.window;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = file;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_DONTADDTORECENT;
	ofn.lpstrDefExt = ".sch";
	if (type == DT_OPEN)
		ofn.Flags |= OFN_PATHMUSTEXIST;
	else if (type == DT_SAVE)
		ofn.Flags |= OFN_OVERWRITEPROMPT;

	bool successful;
	if (type == DT_OPEN)
		successful = GetOpenFileNameA(&ofn);
	else if (type == DT_SAVE)
		successful = GetSaveFileNameA(&ofn);

	if (successful == false)
		return NULL;

	char *result = (char *) malloc(sizeof(char) * (strlen(file) + 1));
	strcpy(result, file);
	return result;
}

#endif
