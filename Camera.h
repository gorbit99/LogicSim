#ifndef HOMEWORK_CAMERA_H
#define HOMEWORK_CAMERA_H

#include "Graphics.h"

typedef struct Camera {
    Point position;
    float zoom;
} Camera;

void camera_zoom(Camera *camera, float zoomLevels, Point zoomPos);
void camera_move(Camera *camera, Vec movement);

#endif //HOMEWORK_CAMERA_H