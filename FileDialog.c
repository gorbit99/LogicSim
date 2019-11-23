#include "FileDialog.h"
#include "debugmalloc.h"

#ifdef __WINDOWS__

char *open_file_dialog(SDL_Window *owner, const char *filter, const char *title, DialogType type) {
	char file[MAX_PATH];

	SDL_SysWMinfo wMinfo;
	SDL_GetWindowWMInfo(owner, &wMinfo);
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
	ofn.lpstrDefExt = ".sav";
	if (type == DT_OPEN)
		ofn.Flags |= OFN_PATHMUSTEXIST;
	else if (type == DT_SAVE)
		ofn.Flags |= OFN_OVERWRITEPROMPT;

	if (type == DT_OPEN)
		GetOpenFileNameA(&ofn);
	else if (type == DT_SAVE)
		GetSaveFileNameA(&ofn);

	char *result = (char *) malloc(sizeof(char) * (strlen(file) + 1));
	strcpy(result, file);
	return result;
}

#endif
