#include "Input.h"
#include "debugmalloc.h"

void input_reset_events(Input *input) {
	for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
		input->buttons[i].isPressed = false;
		input->buttons[i].isReleased = false;
	}

	for (int i = 0; i < 5; i++) {
		input->mouseButtons[i].isReleased = false;
		input->mouseButtons[i].isPressed = false;
		input->mouseClicks[i] = 0;
	}
	input->mouseDeltaX = 0;
	input->mouseDeltaY = 0;
	input->mouseWheelX = 0;
	input->mouseWheelY = 0;
}

void input_handle_event(Input *input, SDL_Event *e) {
    if (e->type == SDL_KEYDOWN) {
		if (!input->buttons[e->key.keysym.scancode].isHeld) {
			input->buttons[e->key.keysym.scancode].isHeld = true;
			input->buttons[e->key.keysym.scancode].isPressed = true;
		}
    } else if (e->type == SDL_KEYUP) {
		if (input->buttons[e->key.keysym.scancode].isHeld) {
			input->buttons[e->key.keysym.scancode].isHeld = false;
			input->buttons[e->key.keysym.scancode].isReleased = true;
		}
    }
    switch (e->type) {
    	case SDL_KEYDOWN:
			if (!input->buttons[e->key.keysym.scancode].isHeld) {
				input->buttons[e->key.keysym.scancode].isHeld = true;
				input->buttons[e->key.keysym.scancode].isPressed = true;
			}
			break;
    	case SDL_KEYUP:
			if (input->buttons[e->key.keysym.scancode].isHeld) {
				input->buttons[e->key.keysym.scancode].isHeld = false;
				input->buttons[e->key.keysym.scancode].isReleased = true;
			}
			break;
    	case SDL_MOUSEBUTTONDOWN:
    		if (!input->mouseButtons[e->button.button].isHeld) {
			    input->mouseButtons[e->button.button].isHeld = true;
			    input->mouseButtons[e->button.button].isPressed = true;
			    input->mouseClicks[e->button.button] = e->button.clicks;
    		}
    		break;
    	case SDL_MOUSEBUTTONUP:
			if (input->mouseButtons[e->button.button].isHeld) {
				input->mouseButtons[e->button.button].isHeld = false;
				input->mouseButtons[e->button.button].isReleased = true;
			}
			break;
    	case SDL_MOUSEMOTION:
		    input->mouseDeltaX += e->motion.xrel;
		    input->mouseDeltaY += e->motion.yrel;
		    input->mouseX = e->motion.x;
		    input->mouseY = e->motion.y;
    		break;
    	case SDL_MOUSEWHEEL:
		    input->mouseWheelX += e->wheel.x;
		    input->mouseWheelY += e->wheel.y;
    		if (e->wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
			    input->mouseWheelX *= -1;
			    input->mouseWheelY *= -1;
    		}
    		break;
    }
}

ButtonState input_get_key(Input *input, SDL_Scancode code) {
	return input->buttons[code];
}

ButtonState input_get_mouse_button(Input *input, int id) {
	return input->mouseButtons[id];
}

int input_get_mouse_x(Input *input) {
	return input->mouseX;
}

int input_get_mouse_y(Input *input) {
	return input->mouseY;
}

int input_get_mouse_delta_x(Input *input) {
	return input->mouseDeltaX;
}

int input_get_mouse_delta_y(Input *input) {
	return input->mouseDeltaY;
}

int input_get_mouse_wheel_x(Input *input) {
	return input->mouseWheelX;
}

int input_get_mouse_wheel_y(Input *input) {
	return input->mouseWheelY;
}

Point input_get_mouse_pos(Input *input) {
	return (Point) {
		(float)input_get_mouse_x(input),
		(float)input_get_mouse_y(input)
	};
}

Vec input_get_mouse_delta(Input *input) {
	return (Vec) {
		(float)input_get_mouse_delta_x(input),
		(float)input_get_mouse_delta_y(input)
	};
}

Vec input_get_mouse_wheel(Input *input) {
	return (Vec) {
		(float)input_get_mouse_wheel_x(input),
		(float)input_get_mouse_wheel_y(input)
	};
}

bool input_mouse_over(Input *input, SDL_Rect r) {
	SDL_Point p = {input_get_mouse_x(input), input_get_mouse_y(input)};
	return SDL_PointInRect(&p, &r) == SDL_TRUE;
}