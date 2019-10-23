#include "Node.h"
#include "debugmalloc.h"

void node_create(Node *node, char *compName, Point pos, TTF_Font *font, SDL_Renderer *renderer) {
    node->component = component_create(pos.x, pos.y, compName, 500, 15, font, renderer);
    node->connections = (Connection *)malloc(sizeof(Connection) * node->component.funData.outC);
    for (int i = 0; i < node->component.funData.outC; i++) {
        node->connections[i].other = NULL;
        node->connections[i].pinB = -1;
    }
    node->inValues = (bool *)malloc(sizeof(bool) * node->component.funData.inC);
    node->outValues = (bool *)malloc(sizeof(bool) * node->component.funData.outC);
    node->dirty = true;
}

void node_set_connection(Node *node, int pinA, Node *other, int pinB) {
    node->connections[pinA].pinB = pinB;
    node->connections[pinA].other = other;
}

void node_set_inval(Node *node, int pinIn, bool value) {
    if (node->inValues[pinIn] != value) {
        node->inValues[pinIn] = value;
        node->dirty = true;
    }
}

void node_update(Node *node) {
    if (!node->dirty)
        return;
    node->dirty = false;
    
}

void node_free(Node *node) {
    free(node->connections);
    free(node->inValues);
    free(node->outValues);
}