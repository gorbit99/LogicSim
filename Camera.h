#ifndef HOMEWORK_CAMERA_H
#define HOMEWORK_CAMERA_H

/**
 * @file Camera.h
 * @brief Structures and information related to the camera system
 * @version 0.1
 * @date 2019-11-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "Graphics.h"

/**
 * @brief Stores camera data
 * 
 */
typedef struct Camera {
    Point position; /**< Position */
    float zoom; /**< Zoom amount (0 - 1) */
} Camera;

/**
 * @brief Zooms in camera to the zoom position
 * 
 * @param camera The camera struct to zoom with
 * @param zoomLevels Relatively how much to zoom
 * @param zoomPos Position to zoom in to
 */
void camera_zoom(Camera *camera, float zoomLevels, Point zoomPos);

/**
 * @brief Moves camera taking the zoom into account
 * 
 * @param camera The camera struct to move with
 * @param movement Amount to move
 */
void camera_move(Camera *camera, Vec movement);

/**
 * @brief Get screen position relative to the world
 * 
 * @param camera The camera struct to do the calculations with
 * @param p The point to transform
 * @return Point The resulting point in world space
 */
Point camera_screen_to_view(Camera *camera, Point p);

#endif //HOMEWORK_CAMERA_H