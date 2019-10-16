#ifndef HOMEWORK_NODE_H
#define HOMEWORK_NODE_H

#include "Component.h"

typedef struct Node {
    ComponentData component;
    struct Node *next;
} Node;

#endif //HOMEWORK_NODE_H