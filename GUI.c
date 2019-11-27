#include "GUI.h"
#include "debugmalloc.h"


void button_render(Button *button, SDL_Renderer *renderer, Point mousePos) {
    if (button_is_over(button, mousePos)) 
        SDL_SetTextureColorMod(button->buttonImage, 255, 128, 255);
    else
        SDL_SetTextureColorMod(button->buttonImage, 255, 255, 255);
    SDL_Rect symbolRect;
    SDL_QueryTexture(button->buttonImage, NULL, NULL, &symbolRect.w, &symbolRect.h);
    symbolRect.x = button->rect.x + (button->rect.w - symbolRect.w) / 2;
    symbolRect.y = button->rect.y + (button->rect.h - symbolRect.h) / 2;
    SDL_RenderCopy(renderer, button->buttonImage, NULL, &symbolRect);
}

bool button_is_over(Button *button, Point mousePos) {
    SDL_Point point = {(int)mousePos.x, (int)mousePos.y};
    return SDL_PointInRect(&point, &button->rect);
}