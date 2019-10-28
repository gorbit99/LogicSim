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