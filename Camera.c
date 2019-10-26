#include "Camera.h"
#include "debugmalloc.h"

static const float minZoom = 0.1f;
static const float maxZoom = 1;

void camera_zoom(Camera *camera, float zoomLevels, Point zoomPos) {
    float multiplier = powf(0.9f, -zoomLevels);

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);

    camera->zoom *= multiplier;
    if (camera->zoom < minZoom) {
        camera->zoom = minZoom;
    }
    else if (camera->zoom > maxZoom) {
        camera->zoom = maxZoom;
    }
    else {
        camera->position.x -= (zoomPos.x - zoomPos.x * multiplier) / camera->zoom;
        camera->position.y -= (zoomPos.y - zoomPos.y * multiplier) / camera->zoom;
    }
}

void camera_move(Camera *camera, Vec movement) {
    camera->position.x -= movement.x / camera->zoom;
    camera->position.y -= movement.y / camera->zoom;
}