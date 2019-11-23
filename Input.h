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
 * @brief Struct holding input information
 */
typedef struct Input {
	ButtonState buttons[SDL_NUM_SCANCODES]; /**< Button states */
	ButtonState mouseButtons[5]; /**< Mouse states */
	uint8_t mouseClicks[5]; /**< Number of clicks on each button */

	int mouseDeltaX, /**< Mouse delta in the X direction */
		mouseDeltaY; /**< Mouse delta in the Y direction */
	int mouseX, /**< Mouse X position */
		mouseY; /**< Mouse Y position */
	int mouseWheelX, /**< Mouse wheel X movement */
		mouseWheelY; /**< Mouse wheel Y movement */
} Input;

/**
 * @brief Start a new event handling
 *
 * @param input Input struct to reset
 */
void input_reset_events(Input *input);

/**
 * @brief Handle event
 * 
 * @param input Input struct to write information to
 * @param e Event to handle
 */
void input_handle_event(Input *input, SDL_Event *e);

/**
 * @brief Get the state of the given key
 *
 * @param input Input struct to get information from
 * @param code Scancode of the key
 * @return ButtonState Resulting state 
 */
ButtonState input_get_key(Input *input, SDL_Scancode code);

/**
 * @brief Get the state of the given mouse button
 *
 * @param input Input struct to get information from
 * @param id Id of the mouse button
 * @return ButtonState Resulting state
 */
ButtonState input_get_mouse_button(Input *input, int id);

/**
 * @brief Get mouse X position
 *
 * @param input Input struct to get information from
 * @return int Mouse X position
 */
int input_get_mouse_x(Input *input);

/**
 * @brief Get mouse Y position
 *
 * @param input Input struct to get information from
 * @return int Mouse Y position
 */
int input_get_mouse_y(Input *input);

/**
 * @brief Get the mouse position as a point
 *
 * @param input Input struct to get information from
 * @return Point Mouse position
 */
Point input_get_mouse_pos(Input *input);

/**
 * @brief Get the mouse delta X between this and the last frame
 *
 * @param input Input struct to get information from
 * @return int Mouse dleta X
 */
int input_get_mouse_delta_x(Input *input);

/**
 * @brief Get the mouse delta Y between this and the last frame
 *
 * @param input Input struct to get information from
 * @return int Mouse delta Y
 */
int input_get_mouse_delta_y(Input *input);

/**
 * @brief Get the mouse delta as a vector
 *
 * @param input Input struct to get information from
 * @return Vec Mouse delta
 */
Vec input_get_mouse_delta(Input *input);

/**
 * @brief Get the mouse wheel movement in the horizontal direction
 *
 * @param input Input struct to get information from
 * @return int Mouse wheel X
 */
int input_get_mouse_wheel_x(Input *input);

/**
 * @brief Get the mouse wheel movement in the vertical direction
 *
 * @param input Input struct to get information from
 * @return int Mouse wheel Y
 */
int input_get_mouse_wheel_y(Input *input);

/**
 * @brief Get the mouse wheel as a vector
 *
 * @param input Input struct to get information from
 * @return Vec Mouse wheel delta
 */
Vec input_get_mouse_wheel(Input *input);

/**
 * @brief Check, if the mouse is over a rect in screen space
 *
 * @param input Input to get t
 * @param r Rect to check
 * @return If the mouse is over the rect or not
 */
bool input_mouse_over(Input *input, SDL_Rect r);

#endif //HOMEWORK_INPUT_H