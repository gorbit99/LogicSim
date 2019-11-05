#ifndef HOMEWORK_INPUT_H
#define HOMEWORK_INPUT_H

/**
 * @file Input.h
 * @brief Structures and functions related to input
 * @version 0.1
 * @date 2019-11-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <SDL.h>
#include <stdbool.h>
#include "Graphics.h"

/**
 * @brief State of a button
 * 
 */
typedef struct ButtonState {
	bool isPressed; /**< Was button just pressed */
	bool isHeld; /**< Is button held */
	bool isReleased; /**< Was button just released */
} ButtonState;

/**
 * @brief Start a new event handling
 * 
 */
void input_reset_events();

/**
 * @brief Handle event
 * 
 * @param e Event to handle
 */
void input_handle_event(SDL_Event *e);

/**
 * @brief Get the state of the given key
 * 
 * @param code Scancode of the key
 * @return ButtonState Resulting state 
 */
ButtonState input_get_key(SDL_Scancode code);

/**
 * @brief Get the state of the given mouse button
 * 
 * @param id Id of the mouse button
 * @return ButtonState Resulting state
 */
ButtonState input_get_mouse_button(int id);

/**
 * @brief Get mouse X position
 * 
 * @return int Mouse X position
 */
int input_get_mouse_x();

/**
 * @brief Get mouse Y position
 * 
 * @return int Mouse Y position
 */
int input_get_mouse_y();

/**
 * @brief Get the mouse position as a point
 * 
 * @return Point Mouse position
 */
Point input_get_mouse_pos();

/**
 * @brief Get the mouse delta X between this and the last frame
 * 
 * @return int Mouse dleta X
 */
int input_get_mouse_delta_x();

/**
 * @brief Get the mouse delta Y between this and the last frame
 * 
 * @return int Mouse delta Y
 */
int input_get_mouse_delta_y();

/**
 * @brief Get the mouse delta as a vector
 * 
 * @return Vec Mouse delta
 */
Vec input_get_mouse_delta();

/**
 * @brief Get the mouse wheel movement in the horizontal direction
 * 
 * @return int Mouse wheel X
 */
int input_get_mouse_wheel_x();

/**
 * @brief Get the mouse wheel movement in the vertical direction
 * 
 * @return int Mouse wheel Y
 */
int input_get_mouse_wheel_y();

/**
 * @brief Get the mouse wheel as a vector
 * 
 * @return Vec Mouse wheel delta
 */
Vec input_get_mouse_wheel();

bool input_mouse_over(SDL_Rect r);

#endif //HOMEWORK_INPUT_H