#ifndef HOMEWORK_NODE_H
#define HOMEWORK_NODE_H

#include "Component.h"
#include "Input.h"
#include "Wire.h"

struct Node;
typedef struct Wire Wire;

typedef struct Node {
    ComponentData component;
    Wire *wires;
    bool *inValues;
    bool *nextInValues;
    bool *outValues;
    SDL_Renderer *renderer;
} Node;

Node node_create(char *compName, Point pos, TTF_Font *font, SDL_Renderer *renderer);
Node node_create_LED(Point pos, SDL_Renderer *renderer);
Node node_create_switch(Point pos, SDL_Renderer *renderer);

void node_add_connection(Node *node, int pinA, Node *other, int pinB);

void node_set_inval(Node *node, int pinIn, bool value);

void node_update_values(Node *node);

void node_run(Node *node);

void node_render(Node *node, Point camPos);

void node_free(Node *node);

bool node_is_over(Node *node, Point p);

#endif //HOMEWORK_NODE_H