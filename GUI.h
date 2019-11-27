#ifndef HOMEWORK_GUI_H
#define HOMEWORK_GUI_H

#include "GUIGraphics.h"
#include "Graphics.h"

typedef struct Button {
    SDL_Rect rect;
    NSliceTexture *bg;
    SDL_Texture *buttonImage;
} Button;

void button_render(Button *button, SDL_Renderer *renderer, Point mousePos);

bool button_is_over(Button *button, Point mousePos);

#endif //HOMEWORK_GUI_H