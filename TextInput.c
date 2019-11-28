#include "TextInput.h"
#include "debugmalloc.h"

TextInput textinput_create() {
	TextInput result;
	result.length = 1;
	result.text = (char *) malloc(sizeof(char));
	result.text[0] = '\0';
	result.selection = 0;
	result.cursorPosition = 0;
	result.cursorPosInPx = 0;
	result.lastUpdate = SDL_GetTicks();
	result.surface = NULL;
	return result;
}

void textinput_start() {
	SDL_StartTextInput();
}

bool textinput_handle_event(TextInput *textinput, SDL_Event *e) {
	if (e->type == SDL_TEXTINPUT) {
		size_t addedLength = strlen(e->text.text);
		char *newMem = realloc(textinput->text, textinput->length + addedLength);
		if (newMem == NULL) {
			log_error("Couldn't allocate new memory!\n");
			return false;
		}
		textinput->text = newMem;
		for (int i = (int) textinput->length - 1; i >= (int) textinput->cursorPosition; i--)
			textinput->text[i + addedLength] = textinput->text[i];
		for (size_t i = 0; i < addedLength; i++)
			textinput->text[textinput->cursorPosition + i] = e->text.text[i];
		textinput->length += addedLength;
		textinput->cursorPosition += addedLength;
		textinput->lastUpdate = SDL_GetTicks();
		return true;
	} else if (e->type == SDL_KEYDOWN) {
		if (e->key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
			if (textinput->cursorPosition > 0) {
				int deleteCount = 1;
				if (e->key.keysym.mod & ((unsigned)KMOD_LCTRL | (unsigned)KMOD_RCTRL)) {
					while (textinput->cursorPosition - deleteCount > 0 &&
							!isspace(textinput->text[textinput->cursorPosition - deleteCount - 1]))
						deleteCount++;
				}
				for (int i = textinput->cursorPosition; i < (int) textinput->length; i++)
					textinput->text[i - deleteCount] = textinput->text[i];
				textinput->text = realloc(textinput->text, textinput->length - deleteCount);
				textinput->length -= deleteCount;
				textinput->cursorPosition -= deleteCount;
			}
			textinput->lastUpdate = SDL_GetTicks();
			return true;
		}
		if (e->key.keysym.scancode == SDL_SCANCODE_DELETE) {
			if (textinput->cursorPosition < textinput->length - 1) {
				int deleteCount = 1;
				if (e->key.keysym.mod & ((unsigned)KMOD_LCTRL | (unsigned)KMOD_RCTRL)) {
					while (textinput->cursorPosition + deleteCount < textinput->length - 1 &&
							!isspace(textinput->text[textinput->cursorPosition + deleteCount - 1]))
						deleteCount++;
				}
				for (int i = (int) textinput->cursorPosition + deleteCount; i < (int) textinput->length; i++)
					textinput->text[i - deleteCount] = textinput->text[i];
				textinput->text = realloc(textinput->text, textinput->length - deleteCount);
				textinput->length -= deleteCount;
			}
			textinput->lastUpdate = SDL_GetTicks();
			return true;
		}
		if (e->key.keysym.scancode == SDL_SCANCODE_LEFT) {
			if (textinput->cursorPosition > 0) {
				textinput->cursorPosition--;
				if (e->key.keysym.mod & ((unsigned)KMOD_LCTRL | (unsigned)KMOD_RCTRL)) {
					while (textinput->cursorPosition > 0 &&
					       !isspace(textinput->text[textinput->cursorPosition - 1]))
						textinput->cursorPosition--;
				}
			}
			textinput->lastUpdate = SDL_GetTicks();
			return false;
		}
		if (e->key.keysym.scancode == SDL_SCANCODE_RIGHT) {
			if (textinput->cursorPosition < textinput->length - 1) {
				textinput->cursorPosition++;
				if (e->key.keysym.mod & ((unsigned)KMOD_LCTRL | (unsigned)KMOD_RCTRL)) {
					while (textinput->cursorPosition < textinput->length - 1 &&
							!isspace(textinput->text[textinput->cursorPosition - 1]))
						textinput->cursorPosition++;
				}
			}
			textinput->lastUpdate = SDL_GetTicks();
			return false;
		}
	}
	return false;
}

void textinput_end() {
	SDL_StopTextInput();
}

void textinput_clear(TextInput *textinput) {
	textinput_free(textinput);
	*textinput = textinput_create();
}

void textinput_free(TextInput *textinput) {
	free(textinput->text);
	SDL_FreeSurface(textinput->surface);
}

void textinput_update_graphic(TextInput *textinput, TTF_Font *font) {
	char temp = textinput->text[textinput->cursorPosition];
	textinput->text[textinput->cursorPosition] = '\0';
	TTF_SizeText(font, textinput->text, &textinput->cursorPosInPx, NULL);
	textinput->text[textinput->cursorPosition] = temp;
	SDL_FreeSurface(textinput->surface);
	textinput->surface = TTF_RenderText_Blended(font, textinput->text, (SDL_Color){0, 0, 0, 255});
}

SDL_Rect textinput_get_cursor_rect(TextInput *textinput, TTF_Font *font, int x, int y) {
	SDL_Rect result = {x + textinput->cursorPosInPx, y, 1, 0};
	if (textinput->surface)
		result.h = textinput->surface->h;
	else
		TTF_SizeText(font, " ", NULL, &result.h);
	return result;
}

void textinput_render(TextInput *textinput, TTF_Font *font, int x, int y, SDL_Renderer *renderer) {
	if (textinput->surface) {
		SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, textinput->surface);
		SDL_Rect src = {0, 0, textinput->surface->w, textinput->surface->h};
		SDL_Rect dst = {x, y, textinput->surface->w, textinput->surface->h};
		SDL_RenderCopy(renderer, texture, &src, &dst);
		SDL_DestroyTexture(texture);
	}
	if (((SDL_GetTicks() - textinput->lastUpdate) / 500) % 2 == 0) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_Rect cursorRect = textinput_get_cursor_rect(textinput, font, x, y);
		SDL_RenderDrawRect(renderer, &cursorRect);
	}
}
