#ifndef HOMEWORK_WIRE_H
#define HOMEWORK_WIRE_H

/**
 * @file Wire.h
 * @brief Functions and structures related to wiring between nodes
 * @version 0.1
 * @date 2019-11-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "Node.h"

typedef struct Node Node;

/**
 * @brief Structure holding information about a connection
 * 
 */
typedef struct Connection {
    int dest; /**< Destination node */
    int pin; /**< Destination pin */
    ComponentData wireComp; /**< Wire corresponding to the connection */
} Connection;

/**
 * @brief Structure holding information about a wire
 * 
 */
typedef struct Wire {
    Node *origin; /**< Origin node */
    int originPin; /**< Origin pin */
    Connection *connections; /**< Connections */
    size_t conCount; /**< Connection count */
    size_t conCap; /**< Connection capacity */
} Wire;

/**
 * @brief Create wire for a node
 * 
 * @param origin Origin node
 * @param originPin Origin pin
 * @return Wire Resulting wire
 */
Wire wire_create(Node *origin, int originPin);

/**
 * @brief Add connection to wire
 * 
 * @param wire Wire to add to
 * @param other Other node
 * @param otherPin Other pin
 * @param nodes Array of the nodes
 */
void wire_add(Wire *wire, int other, int otherPin, Node *nodes);

/**
 * @brief Erase connection from wire
 * 
 * @param wire Wire to erase from
 * @param id Index to erase
 */
void wire_erase(Wire *wire, int id);

/**
 * @brief Send a value over a wire
 * 
 * @param wire Wire to send through
 * @param value Value to send through
 */
void wire_send_value(Wire *wire, bool value, Node *nodes);

/**
 * @brief Free wire
 * 
 * @param wire Wire to free
 */
void wire_free(Wire *wire);

#endif //HOMEWORK_WIRE_H