#ifndef HOMEWORK_COMPONENT_H
#define HOMEWORK_COMPONENT_H

/**
 * @file Component.h
 * @brief Structures and functions related to components
 * @version 0.1
 * @date 2019-11-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <SDL.h>
#include <stdio.h>
#include <math.h>

#include "Utility.h"
#include "Graphics.h"
#include "Parser.h"

extern const float COMPSIZE; /**< Size of each component */
extern const float COMPTHICKNESS; /**< Thickness of the lines */

/**
 * @brief Type of a pin
 * 
 */
typedef enum PinType {
	PIN_IN, /**< Input pin */
	PIN_OUT /** < Output pin */
} PinType;

/**
 * @brief Type of a component
 * 
 */
typedef enum ComponentType {
	CT_MODULE, /**< Module */
	CT_WIRE, /**< Wire */
	CT_SWITCH, /**< Switch */ 
	CT_LED /**< Led */
} ComponentType;

/**
 * @brief Data about a single pin
 * 
 */
typedef struct Pin {
	char name[17]; /**< Name of pin (display if a module) */
	PinType type; /**< Type of the pin */
	float angle; /**< Angle of the wire coming out of the pin */
	Point pos; /**< Position of the pin relative to the component */
} Pin;

/**
 * @brief Data of all pins on a component
 * 
 */
typedef struct PinData {
	Pin *pins; /**< Array of all pins */
	int pinCount; /**< Number of pins */
} PinData;

/**
 * @brief Component
 * 
 */
typedef struct ComponentData {
	float x, /**< X Position of the component */
		  y; /**< Y Position of the component */
	int w, /**< Width of the component */ 
		h; /**< Height of the component */
	SDL_Texture *texture; /**< Texture for the component */
	PinData pinData; /**< Pins of the component */
	FunctionData funData; /**< Function of the component */
	ComponentType type; /**< Type of the component */
} ComponentData;

/**
 * @brief Load the component's corresponding texture
 * 
 * @param path Path to the .cmp file
 * @param size Size to draw the component with
 * @param thickness Thickness to draw the lines with
 * @param font Font to draw the text with
 * @return SDL_Surface* The resulting graphic
 */
SDL_Surface *component_load_graphic(const char *path, float size, float thickness, TTF_Font *font);

/**
 * @brief Load the pin information for the component
 * 
 * @param path Path to the .dat file
 * @param size Size of the component
 * @return PinData The resulting pindata
 */
PinData component_load_pin_data(const char *path, float size);

/**
 * @brief Create a generic component
 * 
 * @param x X position of the component
 * @param y Y position of the component
 * @param name File names for the component
 * @param size Size of the component
 * @param thickness Thickness of the lines
 * @param font Font of the text
 * @param renderer Renderer to send the texture to
 * @return ComponentData Resulting component
 */
ComponentData component_create(float x, float y, char *name, float size, float thickness,
		TTF_Font *font, SDL_Renderer *renderer);

/**
 * @brief Free component
 * 
 * @param dat Component to free
 */
void component_free_data(ComponentData *dat);

/**
 * @brief Render component
 * 
 * @param dat Component to render
 * @param renderer Renderer to render component with
 * @param camPos Position of the camera
 * @param color Color of the component
 */
void component_render(ComponentData *dat, SDL_Renderer *renderer, Point camPos, Color color);

/**
 * @brief Create a texture for a wire between 2 points
 * 
 * @param V1 First position
 * @param V2 Second position
 * @param ang1 First angle
 * @param ang2 Second angle
 * @param size Size of the wire
 * @param thickness Thickness of the line
 * @param pin1Pos After the call, holds the position of the first pin relative to the surface
 * @return SDL_Surface* Resulting graphic
 */
SDL_Surface *component_create_wire_texture(Point V1, Point V2, float ang1, float ang2, float size, float thickness, Point *pin1Pos);

/**
 * @brief Create a wire component between two pins on the two specified components
 * 
 * @param comp1 First component
 * @param comp2 Second component
 * @param pin1 First pin
 * @param pin2 Second pin
 * @param size Size of the wire
 * @param thickness Thickness of the wire
 * @param renderer Renderer to send the texture to
 * @return ComponentData Resulting component
 */
ComponentData component_create_wire_between(ComponentData *comp1, ComponentData *comp2, int pin1, int pin2,
		float size, float thickness, SDL_Renderer *renderer);

/**
 * @brief Create an LED component
 * 
 * @param x X position
 * @param y Y position
 * @param size Size of the component
 * @param thickness Thickness of the lines
 * @param renderer Renderer to send the texture to
 * @return ComponentData Resulting component
 */
ComponentData component_create_LED(float x, float y, float size, float thickness, SDL_Renderer *renderer);

/**
 * @brief Create a Switch component
 * 
 * @param x X position
 * @param y Y position
 * @param size Size of the component
 * @param thickness Thickness of the lines
 * @param renderer Renderer to send the texture to
 * @return ComponentData Resulting component
 */
ComponentData component_create_switch(float x, float y, float size, float thickness, SDL_Renderer *renderer);

/**
 * @brief Simulate component based on input
 * 
 * @param component Component to simulate
 * @param in Input values
 * @param out Where to write output
 */
void component_run(ComponentData *component, bool *in, bool *out);

#endif //HOMEWORK_COMPONENT_H
