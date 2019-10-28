#include "Input.h"
#include "debugmalloc.h"

static ButtonState buttons[SDL_NUM_SCANCODES];
static ButtonState mouseButtons[5];
static uint8_t mouseClicks[5];

static int mouseDeltaX = 0, mouseDeltaY = 0;
static int mouseX = 0, mouseY = 0;
static int mouseWheelX = 0, mouseWheelY = 0;

void input_reset_events() {
	for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
		buttons[i].isPressed = false;
		buttons[i].isReleased = false;
	}

	for (int i = 0; i < 5; i++) {
		mouseButtons[i].isReleased = false;
		mouseButtons[i].isPressed = false;
		mouseClicks[i] = 0;
	}
	mouseDeltaX = 0;
	mouseDeltaY = 0;
	mouseWheelX = 0;
	mouseWheelY = 0;
}

void input_handle_event(SDL_Event *e) {
    if (e->type == SDL_KEYDOWN) {
		if (!buttons[e->key.keysym.scancode].isHeld) {
			buttons[e->key.keysym.scancode].isHeld = true;
			buttons[e->key.keysym.scancode].isPressed = true;
		}
    } else if (e->type == SDL_KEYUP) {
		if (buttons[e->key.keysym.scancode].isHeld) {
			buttons[e->key.keysym.scancode].isHeld = false;
			buttons[e->key.keysym.scancode].isReleased = true;
		}
    }
    switch (e->type) {
    	case SDL_KEYDOWN:
			if (!buttons[e->key.keysym.scancode].isHeld) {
				buttons[e->key.keysym.scancode].isHeld = true;
				buttons[e->key.keysym.scancode].isPressed = true;
			}
			break;
    	case SDL_KEYUP:
			if (buttons[e->key.keysym.scancode].isHeld) {
				buttons[e->key.keysym.scancode].isHeld = false;
				buttons[e->key.keysym.scancode].isReleased = true;
			}
			break;
    	case SDL_MOUSEBUTTONDOWN:
    		if (!mouseButtons[e->button.button].isHeld) {
				mouseButtons[e->button.button].isHeld = true;
				mouseButtons[e->button.button].isPressed = true;
				mouseClicks[e->button.button] = e->button.clicks;
    		}
    		break;
    	case SDL_MOUSEBUTTONUP:
			if (mouseButtons[e->button.button].isHeld) {
				mouseButtons[e->button.button].isHeld = false;
				mouseButtons[e->button.button].isReleased = true;
			}
			break;
    	case SDL_MOUSEMOTION:
    		mouseDeltaX += e->motion.xrel;
    		mouseDeltaY += e->motion.yrel;
    		mouseX = e->motion.x;
    		mouseY = e->motion.y;
    		break;
    	case SDL_MOUSEWHEEL:
    		mouseWheelX += e->wheel.x;
    		mouseWheelY += e->wheel.y;
    		if (e->wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
    			mouseWheelX *= -1;
    			mouseWheelY *= -1;
    		}
    		break;
    }
}

ButtonState input_get_key(SDL_Scancode code) {
	return buttons[code];
}

ButtonState input_get_mouse_button(int id) {
	return mouseButtons[id];
}

int input_get_mouse_x() {
	return mouseX;
}

int input_get_mouse_y() {
	return mouseY;
}

int input_get_mouse_delta_x() {
	return mouseDeltaX;
}

int input_get_mouse_delta_y() {
	return mouseDeltaY;
}

int input_get_mouse_wheel_x() {
	return mouseWheelX;
}

int input_get_mouse_wheel_y() {
	return mouseWheelY;
}

Point input_get_mouse_pos() {
	return (Point) {
		(float)input_get_mouse_x(),
		(float)input_get_mouse_y()
	};
}

Vec input_get_mouse_delta() {
	return (Vec) {
		(float)input_get_mouse_delta_x(),
		(float)input_get_mouse_delta_y()
	};
}

Vec input_get_mouse_wheel() {
	return (Vec) {
		(float)input_get_mouse_wheel_x(),
		(float)input_get_mouse_wheel_y()
	};
}

bool input_mouse_over(SDL_Rect r) {
	SDL_Point p = {input_get_mouse_x(), input_get_mouse_y()};
	return SDL_PointInRect(&p, &r) == SDL_TRUE;
}