#include "Node.h"
#include "debugmalloc.h"

Node node_create(char *compName, Point pos, TTF_Font *font, SDL_Renderer *renderer) {
    Node node;
	node.component = component_create(pos.x, pos.y, compName, 300, 15, font, renderer);
    node.connections = (Connection *)malloc(sizeof(Connection) * node.component.funData.outC);
    for (int i = 0; i < node.component.funData.outC; i++) {
        node.connections[i].other = NULL;
        node.connections[i].pinB = -1;
    }
    node.inValues = (bool *)calloc(node.component.funData.inC, sizeof(bool));
	node.prevInValues = (bool *)calloc(node.component.funData.inC, sizeof(bool));
    node.outValues = (bool *)calloc(node.component.funData.outC, sizeof(bool));
    node.dirty = false;
    node.prevDirty = false;
    node.renderer = renderer;
    return node;
}

Node node_create_LED(Point pos, SDL_Renderer *renderer) {
	Node node;
	node.component = component_create_LED(pos.x, pos.y, 300, 15, renderer);
	node.connections = (Connection *)malloc(sizeof(Connection) * node.component.funData.outC);
	for (int i = 0; i < node.component.funData.outC; i++) {
		node.connections[i].other = NULL;
		node.connections[i].pinB = -1;
	}
	node.inValues = (bool *)calloc(node.component.funData.inC, sizeof(bool));
	node.prevInValues = (bool *)calloc(node.component.funData.inC, sizeof(bool));
	node.outValues = (bool *)calloc(node.component.funData.outC, sizeof(bool));
	node.dirty = false;
	node.prevDirty = false;
	node.renderer = renderer;
	return node;
}

Node node_create_switch(Point pos, SDL_Renderer *renderer) {
	Node node;
	node.component = component_create_switch(pos.x, pos.y, 300, 15, renderer);
	node.connections = (Connection *)malloc(sizeof(Connection) * node.component.funData.outC);
	for (int i = 0; i < node.component.funData.outC; i++) {
		node.connections[i].other = NULL;
		node.connections[i].pinB = -1;
	}
	node.inValues = (bool *)calloc(node.component.funData.inC, sizeof(bool));
	node.prevInValues = (bool *)calloc(node.component.funData.inC, sizeof(bool));
	node.outValues = (bool *)calloc(node.component.funData.outC, sizeof(bool));
	node.dirty = true;
	node.prevDirty = false;
	node.renderer = renderer;
	return node;
}

void node_set_connection(Node *node, int pinA, Node *other, int pinB) {
    node->connections[pinA].pinB = pinB;
    node->connections[pinA].other = other;
    node->connections[pinA].wire = component_create_wire_between(&node->component,
    		&other->component,
    		pinA + node->component.funData.inC,
    		pinB,
    		300,
    		15,
    		node->renderer);
}

void node_set_inval(Node *node, int pinIn, bool value) {
    if (node->inValues[pinIn] != value) {
        node->inValues[pinIn] = value;
        node->dirty = true;
    }
}

void node_update(Node *node) {
    for (int i = 0; i < node->component.funData.assignC; i++) {
    	parser_handle_operation(node->component.funData.assigns[i].op, node->prevInValues, node->outValues + i);
	}
    for (int i = 0; i < node->component.funData.outC; i++) {
    	if (node->connections[i].other != NULL)
    		node_set_inval(node->connections[i].other, node->connections[i].pinB, node->outValues[i]);
    }
}

void node_free(Node *node) {
	component_free_data(&node->component);
    free(node->connections);
    if (node->component.funData.inC > 0)
    	free(node->inValues);
    free(node->prevInValues);
    free(node->outValues);
}

void node_render(Node *node, Point camPos) {
	for (int i = 0; i < node->component.funData.outC; i++) {
		if (node->connections[i].other) {
			if (node->outValues[i])
				component_render(&node->connections[i].wire, node->renderer, camPos, 0xff7700ff);
			else
				component_render(&node->connections[i].wire, node->renderer, camPos, 0x0000aaff);
		}
	}
	if (node->component.type == CT_MODULE)
		component_render(&node->component, node->renderer, camPos, 0xffffffff);
	else if (node->component.type == CT_LED) {
		if (node->inValues[0])
			component_render(&node->component, node->renderer, camPos, 0xffff00ff);
		else
			component_render(&node->component, node->renderer, camPos, 0x555555ff);
	} else if (node->component.type == CT_SWITCH) {
		if (node->outValues[0])
			component_render(&node->component, node->renderer, camPos, 0xff7700ff);
		else
			component_render(&node->component, node->renderer, camPos, 0x0000aaff);
	}
}
