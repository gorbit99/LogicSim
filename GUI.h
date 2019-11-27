#ifndef HOMEWORK_GUI_H
#define HOMEWORK_GUI_H

#include "GUIGraphics.h"
#include "Graphics.h"

typedef struct Button {
    SDL_Rect rect;
    SDL_Texture *buttonImage;
} Button;

Button button_create(SDL_Rect rect, char *path, SDL_Renderer *renderer);

void button_render(Button *button, SDL_Renderer *renderer, Point mousePos);

bool button_is_over(Button *button, Point mousePos);

void button_free(Button *button);

#endif //HOMEWORK_GUI_H