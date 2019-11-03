#ifndef HOMEWORK_NODEVECTOR_H
#define HOMEWORK_NODEVECTOR_H

#include <stddef.h>
#include "NodeEvent.h"

typedef struct NodeVector {
	Node *nodes;
	size_t count;
	size_t capacity;
} NodeVector;

NodeVector nodev_create(size_t count);

void nodev_push_back(NodeVector *vector, Node node);

void nodev_erase(NodeVector *vector, int index);

Node *nodev_at(NodeVector *vector, int index);

void nodev_connect(NodeVector *vector, int idA, int pinA, int idB, int pinB);

void nodev_recalc_levels(NodeVector *vector);

void nodev_switch(NodeVector *vector, int id);

void nodev_update_all(NodeVector *vector);

void nodev_free(NodeVector *vector);

#endif //HOMEWORK_NODEVECTOR_H
