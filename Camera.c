#include "Camera.h"
#include "debugmalloc.h"

static const float minZoom = 0.1f;
static const float maxZoom = 1;

void camera_zoom(Camera *camera, float zoomLevels, Point zoomPos) {
	float multiplier = powf(0.9f, -zoomLevels);

	if (camera->zoom * multiplier < minZoom)
		multiplier = minZoom / camera->zoom;
	else if (camera->zoom * multiplier > maxZoom)
		multiplier = maxZoom / camera->zoom;
	camera->zoom *= multiplier;

	camera->position.x -= (zoomPos.x - zoomPos.x * multiplier) / camera->zoom;
	camera->position.y -= (zoomPos.y - zoomPos.y * multiplier) / camera->zoom;
}

void camera_move(Camera *camera, Vec movement) {
	camera->position.x -= movement.x / camera->zoom;
	camera->position.y -= movement.y / camera->zoom;
}

Point camera_screen_to_view(Camera *camera, Point p) {
	return (Point){(p.x) / camera->zoom + camera->position.x,
		(p.y) / camera->zoom + camera->position.y};
}

void camera_update(Camera *camera, Input *input, SDL_Renderer *renderer) {
	if (input_get_mouse_button(input, SDL_BUTTON_MIDDLE).isHeld)
		camera_move(camera, (Vec) {(float) input_get_mouse_delta_x(input),
		                            (float) input_get_mouse_delta_y(input)});


	SDL_Cursor *cursor = SDL_GetCursor();
	if (input_get_mouse_button(input, SDL_BUTTON_MIDDLE).isPressed) {
		cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
		SDL_SetCursor(cursor);
		SDL_FreeCursor(cursor);
		SDL_CaptureMouse(true);
	}
	if (input_get_mouse_button(input, SDL_BUTTON_MIDDLE).isReleased) {
		cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		SDL_SetCursor(cursor);
		SDL_FreeCursor(cursor);
		SDL_CaptureMouse(false);
	}

	camera_zoom(camera, (float) input_get_mouse_wheel_y(input),
	            input_get_mouse_pos(input));
	SDL_RenderSetScale(renderer, camera->zoom, camera->zoom);
}
