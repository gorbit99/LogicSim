#ifndef HOMEWORK_WIRE_H
#define HOMEWORK_WIRE_H

#include "Node.h"

typedef struct Node Node;

typedef struct Connection {
    Node *dest;
    int pin;
    ComponentData wireComp;
} Connection;

typedef struct Wire {
    Node *origin;
    int originPin;
    Connection *connections;
    size_t conCount;
    size_t conCap;
} Wire;

Wire wire_create(Node *origin, int originPin);

void wire_add(Wire *wire, Node *other, int otherPin);

void wire_erase(Wire *wire, int id);

void wire_send_value(Wire *wire, bool value);

void wire_free(Wire *wire);

#endif //HOMEWORK_WIRE_H