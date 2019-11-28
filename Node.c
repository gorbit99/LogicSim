#include "Node.h"
#include "debugmalloc.h"

Node node_create(char *compName, Point pos, TTF_Font *font, SDL_Renderer *renderer) {
    Node node;
	node.component = component_create(pos.x, pos.y, compName, COMPSIZE, COMPTHICKNESS, font, renderer);
	node.wires = (Wire *)malloc(sizeof(Wire) * node.component.funData.outC);
	for (int i = 0; i < node.component.funData.outC; i++) {
		node.wires[i] = wire_create(&node, i);
	}
	node.inValues = (bool *)calloc(node.component.funData.inC, sizeof(bool));
	node.nextInValues = (bool *)calloc(node.component.funData.inC, sizeof(bool));
	node.outValues = (bool *)calloc(node.component.funData.outC, sizeof(bool));
	node.renderer = renderer;
	if (strcmp(compName, "led") == 0)
		node.component.type = CT_LED;
	else if (strcmp(compName, "switch") == 0)
		node.component.type = CT_SWITCH;
    return node;
}

void node_add_connection(Node *node, int pinA, int other, int pinB, Node *nodes) {
    nodes[other].component.pinData.pins[pinB].occupied = true;
	wire_add(&node->wires[pinA], other, pinB, nodes);
}

void node_set_inval(Node *node, int pinIn, bool value) {
    if (node->inValues[pinIn] != value) {
        node->nextInValues[pinIn] = value;
    }
}

void node_update_values(Node *node) {
	for (int i = 0; i < node->component.funData.inC; i++)
		node->inValues[i] = node->nextInValues[i];
}

void node_run(Node *node, Node *nodes) {
	component_run(&node->component, node->inValues, node->outValues);
	for (int i = 0; i < node->component.funData.outC; i++)
		wire_send_value(&node->wires[i], node->outValues[i], nodes);
}

void node_free(Node *node) {
	component_free_data(&node->component);
    for (int i = 0; i < node->component.funData.outC; i++)
		wire_free(&node->wires[i]);
	free(node->wires);
    if (node->component.funData.inC > 0) {
    	free(node->inValues);
    	free(node->nextInValues);
	}
    free(node->outValues);
}

void node_render(Node *node, Point camPos) {
	for (int i = 0; i < node->component.funData.outC; i++) {
		for (size_t w = 0; w < node->wires[i].conCount; w++) {
			if (node->outValues[i])
				component_render(&node->wires[i].connections[w].wireComp, node->renderer, camPos, 0xff7700ff);
			else
				component_render(&node->wires[i].connections[w].wireComp, node->renderer, camPos, 0x0000aaff);
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
	SDL_Surface *unconnectedS = get_rgba_surface((int)(COMPSIZE * 0.4f), (int)(COMPSIZE * 0.4f));
	gfx_fill_ring(unconnectedS, (Point){COMPSIZE * 0.2f, COMPSIZE * 0.2f}, COMPSIZE * 0.17f, COMPTHICKNESS, 0x0000ffff);
	SDL_Texture *unconnected = SDL_CreateTextureFromSurface(node->renderer, unconnectedS);
	SDL_FreeSurface(unconnectedS);
	for (int i = 0; i < node->component.funData.outC; i++) {
		if (node->wires[i].conCount == 0) {
			Point pos = node->component.pinData.pins[i + node->component.funData.inC].pos;
			SDL_Rect dst = {
					(int)(node->component.x - (float)node->component.w / 2 + pos.x - COMPSIZE * 0.1f - camPos.x),
					(int)(node->component.y - (float)node->component.h / 2 + pos.y - COMPSIZE * 0.1f - camPos.y),
					(int)(COMPSIZE * 0.2f),
					(int)(COMPSIZE * 0.2f)
			};
			SDL_RenderCopy(node->renderer, unconnected, NULL, &dst);
		}
	}
	SDL_DestroyTexture(unconnected);
}

bool node_is_over(Node *node, Point p) {
	SDL_Point sp = {(int)p.x, (int)p.y};
	SDL_Rect rect = {(int)node->component.x - (int)node->component.w / 2,(int)node->component.y - node->component.h / 2, node->component.w, node->component.h};
	return SDL_PointInRect(&sp, &rect);
}

void node_reposition_wires(Node *node, Node *nodes) {
	for (int i = 0; i < node->component.funData.outC; i++)
		wire_reposition(&node->wires[i], nodes);
}