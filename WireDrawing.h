#ifndef HOMEWORK_WIREDRAWING_H
#define HOMEWORK_WIREDRAWING_H

#include "Node.h"
#include "NodeVector.h"

typedef struct WireDrawing {
    Node *origin;
    int originPin;
} WireDrawing;

Point closest_on_wire(Node *nodeA, int pinA, Node *nodeB, int pinB, Point P, float *dist);

float dist_from_pin(Node *node, int pinA, Point P);

Wire *closest_wire(NodeVector *vec, Point P, float *dist, Point *point, size_t *connection);

Node *closest_pin(NodeVector *vec, Point P, int *pin, float *dist);

bool wiredrawing_start(NodeVector *vec, Point mousePos, WireDrawing *wireDrawing);

void wiredrawing_update(WireDrawing *wireDrawing, NodeVector *vec, Point mousePos, Point cameraPos, SDL_Renderer *renderer);

void wiredrawing_end(WireDrawing *wireDrawing, NodeVector *vec, Point mousePos);

#endif //HOMEWORK_WIREDRAWING_H
