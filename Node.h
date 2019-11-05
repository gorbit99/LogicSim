#ifndef HOMEWORK_NODE_H
#define HOMEWORK_NODE_H

/**
 * @file Node.h
 * @brief Structures and functions related to the Logic Node system
 * @version 0.1
 * @date 2019-11-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "Component.h"
#include "Input.h"
#include "Wire.h"

struct Node;
typedef struct Wire Wire;

/**
 * @brief Data about a single node in the logic diagram
 * 
 */
typedef struct Node {
    ComponentData component; /**< Component in the node */
    Wire *wires; /**< Wires coming out of the out pins */
    bool *inValues; /**< Current input values */
    bool *nextInValues; /**< Input values for the next frame */
    bool *outValues; /**< Output values */
    SDL_Renderer *renderer; /**< Renderer associated with the node */
} Node;

/**
 * @brief Create a generic node given a name
 * 
 * @param compName Component name to use
 * @param pos Position of the node
 * @param font Font to render the component with
 * @param renderer Renderer to associate the node with
 * @return Node Resulting node
 */
Node node_create(char *compName, Point pos, TTF_Font *font, SDL_Renderer *renderer);

/**
 * @brief Create an LED node
 * 
 * @param pos Position of the node
 * @param renderer Renderer associated with the node
 * @return Node Resulting node
 */
Node node_create_LED(Point pos, SDL_Renderer *renderer);

/**
 * @brief Create a switch node
 * 
 * @param pos Position of the node
 * @param renderer Renderer associated with the node
 * @return Node Resulting node
 */
Node node_create_switch(Point pos, SDL_Renderer *renderer);

/**
 * @brief Connect node to given node
 * 
 * @param node Output node
 * @param pinA Output pin
 * @param other Input node
 * @param pinB Input pin
 */
void node_add_connection(Node *node, int pinA, Node *other, int pinB);

/**
 * @brief Set the input value of the node for the next frame
 * 
 * @param node Node to set input value for
 * @param pinIn Pin id
 * @param value Value to set
 */
void node_set_inval(Node *node, int pinIn, bool value);

/**
 * @brief Load input values for the next frame
 * 
 * @param node Node to update
 */
void node_update_values(Node *node);

/**
 * @brief Simulate node
 * 
 * @param node Node to simulate
 */
void node_run(Node *node);

/**
 * @brief Render node
 * 
 * @param node Node to render
 * @param camPos Position to render at
 */
void node_render(Node *node, Point camPos);

/**
 * @brief Free node
 * 
 * @param node Node to free
 */
void node_free(Node *node);

/**
 * @brief Is a point over a node
 * 
 * @param node Node to check
 * @param p Point to check
 * @return true The point is over the node
 * @return false The point isn't over the node
 */
bool node_is_over(Node *node, Point p);

#endif //HOMEWORK_NODE_H