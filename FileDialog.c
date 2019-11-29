#include "FileDialog.h"
#include "debugmalloc.h"

#ifdef __WINDOWS__

char *open_file_dialog(SDL_Window *owner, const char *filter, const char *title, DialogType type) {
	char file[MAX_PATH];

	SDL_SysWMinfo wMinfo;
	SDL_VERSION(&wMinfo.version)
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
	ofn.lpstrDefExt = ".sch";
	if (type == DT_OPEN)
		ofn.Flags |= (unsigned)OFN_PATHMUSTEXIST;
	else if (type == DT_SAVE)
		ofn.Flags |= (unsigned)OFN_OVERWRITEPROMPT;

	bool successful = true;
	if (type == DT_OPEN)
		successful = GetOpenFileNameA(&ofn);
	else if (type == DT_SAVE)
		successful = GetSaveFileNameA(&ofn);

	if (!successful)
		return NULL;

	char *result = (char *) malloc(sizeof(char) * (strlen(file) + 1));
	strcpy(result, file);
	return result;
}

#endif

#ifdef __LINUX__

char *open_file_dialog(SDL_Window *owner, const char *filter, const char *title, DialogType type) {
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    char *firstButtonText = NULL;
    if (type == DT_OPEN) {
        action = GTK_FILE_CHOOSER_ACTION_OPEN;
        firstButtonText = "Open";
    }
    else if (type == DT_SAVE) {
        action = GTK_FILE_CHOOSER_ACTION_SAVE;
        firstButtonText = "Save";
    }

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
                title,
                NULL,
                action,
                firstButtonText,
                GTK_RESPONSE_ACCEPT,
                "Cancel",
                GTK_RESPONSE_CANCEL,
                NULL
            );
    char *result = NULL;
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        result = (char *)malloc(sizeof(char) * (strlen(filename) + 1));
        strcpy(result, filename);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
    while (gtk_events_pending())
        gtk_main_iteration();
    return result;
}

#endif