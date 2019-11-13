#ifndef HOMEWORK_WIREDRAWING_H
#define HOMEWORK_WIREDRAWING_H

#include "Node.h"
#include "NodeVector.h"

Point closest_on_wire(Node *nodeA, int pinA, Node *nodeB, int pinB, Point P, float *dist);

float dist_from_pin(Node *node, int pinA, Point P);

Point closest_point_on_wires(NodeVector *vec, float *dist, Point P);

#endif //HOMEWORK_WIREDRAWING_H
