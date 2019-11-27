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

#ifdef __LINUX__

#include <gtk-3.0/gtk/gtk.h>
#include <SDL_syswm.h>

char *open_file_dialog(SDL_Window *owner, const char *filter, const char *title, DialogType type) {

    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    char actionText[20];
    if (type == DT_OPEN) {
        action = GTK_FILE_CHOOSER_ACTION_OPEN;
        strcpy(actionText, "Open");
    }
    else if (type == DT_SAVE) {
        action = GTK_FILE_CHOOSER_ACTION_SAVE;
        strcpy(actionText, "Save");
    }

    GtkWidget *dialog = gtk_file_chooser_dialog_new(title,
            NULL,
            action,
            actionText,
            GTK_RESPONSE_ACCEPT,
            NULL);

    gint res = gtk_dialog_run(GTK_DIALOG(dialog));
    char *filename = NULL;
    if (res == GTK_RESPONSE_ACCEPT) {
        char *gtkfilename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        filename = (char *) malloc(sizeof(char) * (strlen(gtkfilename) + 1));
        strcpy(filename, gtkfilename);
        g_free(gtkfilename);
    }

    gtk_widget_destroy(dialog);
    while (gtk_events_pending())
        gtk_main_iteration();

    return filename;
}

#endif