#ifndef HOMEWORK_INPUT_H
#define HOMEWORK_INPUT_H

#include <SDL.h>
#include <stdbool.h>
#include "Graphics.h"

typedef struct ButtonState {
	bool isPressed;
	bool isHeld;
	bool isReleased;
} ButtonState;

void input_reset_events();
void input_handle_event(SDL_Event *e);
ButtonState input_get_key(SDL_Scancode code);
ButtonState input_get_mouse_button(int id);
int input_get_mouse_x();
int input_get_mouse_y();
Point input_get_mouse_pos();
int input_get_mouse_delta_x();
int input_get_mouse_delta_y();
Vec input_get_mouse_delta();
int input_get_mouse_wheel_x();
int input_get_mouse_wheel_y();
Vec input_get_mouse_wheel();

bool input_mouse_over(SDL_Rect r);

#endif //HOMEWORK_INPUT_H