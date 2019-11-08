#ifndef HOMEWORK_NODEVECTOR_H
#define HOMEWORK_NODEVECTOR_H

/**
 * @file NodeVector.h
 * @brief Structures and functions related to Node Vectors
 * @version 0.1
 * @date 2019-11-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stddef.h>
#include "Node.h"

/**
 * @brief Struct of a vector containing nodes
 * 
 */
typedef struct NodeVector {
	Node *nodes; /**< Nodes in the vector */
	size_t count; /**< Number of nodes in the vector */
	size_t capacity; /**< Capacity of the vector */
} NodeVector;

/**
 * @brief Create a node vector
 * 
 * @param count Number of items originally in the vector
 * @return NodeVector Resulting vector
 */
NodeVector nodev_create(size_t count);

/**
 * @brief Add element to the back of the vector
 * 
 * @param vector Vector to push to
 * @param node Node to push
 */
void nodev_push_back(NodeVector *vector, Node node);

/**
 * @brief Erase an element from the vector
 * 
 * @param vector Vector to erase from
 * @param index Index to erase
 */
void nodev_erase(NodeVector *vector, int index);

/**
 * @brief Get the element of the vector at the specified position
 * 
 * @param vector Vector to get the element from
 * @param index Index to get the element from
 * @return Node* Resulting node
 */
Node *nodev_at(NodeVector *vector, int index);

/**
 * @brief Connect two nodes in the vector
 * 
 * @param vector Vector to connect two nodes in
 * @param idA Id of the first node
 * @param pinA Pin of the first node
 * @param idB Id of the second node
 * @param pinB Pin of the second node
 */
void nodev_connect(NodeVector *vector, int idA, int pinA, int idB, int pinB);

/**
 * @brief Render the nodes inside a vector
 * 
 * @param vector Vector to render
 * @param camPos Camera position to render at
 */
void nodev_render(NodeVector *vector, Point camPos);

/**
 * @brief Run simulation
 * 
 * @param vector Vector to simulate
 */
void nodev_update(NodeVector *vector);

/**
 * @brief Check and handle mouse clicks
 * 
 * @param vector Vector to check
 * @param mousePos Position of the mouse
 */
void nodev_check_clicks(NodeVector *vector, Point mousePos);

/**
 * @brief Free vector
 * 
 * @param vector Vector to free
 */
void nodev_free(NodeVector *vector);

#endif //HOMEWORK_NODEVECTOR_H
