#ifndef HOMEWORK_NODE_H
#define HOMEWORK_NODE_H

#include "Component.h"

struct Node;

typedef struct Connection {
    int pinB;
    struct Node *other;
    ComponentData wire;
} Connection;

typedef struct Node {
    ComponentData component;
    Connection *connections;
    bool *inValues;
    bool *outValues;
    bool dirty;
    SDL_Renderer *renderer;
} Node;

Node node_create(char *compName, Point pos, TTF_Font *font, SDL_Renderer *renderer);
Node node_create_LED(Point pos, SDL_Renderer *renderer);
Node node_create_switch(Point pos, SDL_Renderer *renderer);

void node_set_connection(Node *node, int pinA, Node *other, int pinB);

void node_set_inval(Node *node, int pinIn, bool value);

void node_update(Node *node);

void node_render(Node *node, Point camPos, float zoom);

void node_free(Node *node);

#endif //HOMEWORK_NODE_H