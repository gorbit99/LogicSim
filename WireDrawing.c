#include "WireDrawing.h"
#include "debugmalloc.h"

Point closest_on_wire(Node *nodeA, int pinA, Node *nodeB, int pinB, Point P, float *dist) {
	Point P0 = component_get_pin_position(&nodeA->component, pinA);
	Point P3 = component_get_pin_position(&nodeB->component, pinB);
	float ang1 = nodeA->component.pinData.pins[pinA].angle;
	float ang2 = nodeB->component.pinData.pins[pinB].angle;
	float dx1 = cosf(ang1) * COMPSIZE / 2;
	float dy1 = sinf(ang1) * COMPSIZE / 2;
	float dx2 = cosf(ang2) * COMPSIZE / 2;
	float dy2 = sinf(ang2) * COMPSIZE / 2;
	Point P1 = {P0.x + dx1, P0.y + dy1};
	Point P2 = {P3.x + dx2, P3.y + dy2};

	float t = 0;
	Point minPoint = {};
	float minDistance = INFINITY;
	while (t <= 1) {
		Point Q = {
				(1 - t) * (1 - t) * (1 - t) * P0.x + 3 * (1 - t) * (1 - t) * t * P1.x + 3 * (1 - t) * t * t * P2.x + t * t * t * P3.x,
				(1 - t) * (1 - t) * (1 - t) * P0.y + 3 * (1 - t) * (1 - t) * t * P1.y + 3 * (1 - t) * t * t * P2.y + t * t * t * P3.y
		};
		float d2 = (P.x - Q.x) * (P.x - Q.x) + (P.y - Q.y) * (P.y - Q.y);
		if (d2 < minDistance) {
			minPoint = Q;
			minDistance = d2;
		}
		t += 0.01f;
	}
	*dist = sqrtf(minDistance);
	return minPoint;
}

float dist_from_pin(Node *node, int pinA, Point P) {
	Point pin = component_get_pin_position(&node->component, pinA + node->component.funData.inC);
	return sqrtf((pin.x - P.x) * (pin.x - P.x) + (pin.y - P.y) * (pin.y - P.y));
}

Wire *closest_wire(NodeVector *vec, Point P, float *dist, Point *point, size_t *connection) {
	Wire *closest = NULL;
	*dist = INFINITY;
	for (size_t n = 0; n < vec->count; n++) {
		Node *node = nodev_at(vec, n);
		for (int w = 0; w < node->component.funData.outC; w++) {
			Wire *wire = &node->wires[w];
			for (size_t c = 0; c < wire->conCount; c++) {
				Connection *conn = &wire->connections[c];
				Node *dest = nodev_at(vec, conn->dest);
				float curDist;
				Point curPoint = closest_on_wire(node, wire->originPin + node->component.funData.inC, dest, conn->pin, P, &curDist);
				if (curDist < *dist) {
					*point = curPoint;
					*dist = curDist;
					closest = wire;
					*connection = c;
				}
			}
		}
	}
	return closest;
}

Node *closest_pin(NodeVector *vec, Point P, int *pin, float *dist) {
	*dist = INFINITY;
	Node *node = NULL;
	*pin = 0;

	for (size_t n = 0; n < vec->count; n++)
	{
		Node *curNode = nodev_at(vec, n);
		for (int p = 0; p < curNode->component.funData.outC; p++)
		{
			float curDist = dist_from_pin(curNode, p, P);
			if (curDist < *dist) {
				node = curNode;
				*dist = curDist;
				*pin = p + curNode->component.funData.inC;
			}
		}
	}
	return node;
}

bool wiredrawing_start(NodeVector *vec, Point mousePos, WireDrawing *wireDrawing) {
	int pin;
	float pinDist;
	Node *pinNode = closest_pin(vec, mousePos, &pin, &pinDist);

	Point wirePoint;
	float wireDist;
	size_t connection;
	Wire *wire = closest_wire(vec, mousePos, &wireDist, &wirePoint, &connection);

	if (wireDist > 30 && pinDist > 30)
		return false;

	if (pinDist <= 30) {
		wireDrawing->origin = pinNode;
		wireDrawing->originPin = pin;
	} else {
		wireDrawing->origin = wire->origin;
		wireDrawing->originPin = wire->originPin + wire->origin->component.funData.inC;
		nodev_at(vec, wire->connections[connection].dest)->component.pinData.pins[wire->connections[connection].pin].occupied = false;
		wire_erase(wire, connection);
	}
	return true;
}

void wiredrawing_update(WireDrawing *wireDrawing, NodeVector *vec, Point mousePos, Point cameraPos, SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
	Point pin = component_get_pin_position(&wireDrawing->origin->component, wireDrawing->originPin);
	Point pin1Pos;
	SDL_Surface *wire = component_create_wire_texture(pin, mousePos, wireDrawing->origin->component.pinData.pins[wireDrawing->originPin].angle, 3.1415f, COMPSIZE, COMPTHICKNESS, &pin1Pos);
	SDL_Rect dst = {
		(int)(pin.x - pin1Pos.x - cameraPos.x),
		(int)(pin.y - pin1Pos.y - cameraPos.y),		
		wire->w,
		wire->h
	};
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, wire);
	SDL_SetTextureColorMod(texture, 0, 200, 0);
	SDL_RenderCopy(renderer, texture, NULL, &dst);
	SDL_FreeSurface(wire);
	SDL_DestroyTexture(texture);
	
	SDL_Surface *unconnectedS = get_rgba_surface((int)(COMPSIZE * 0.4f), (int)(COMPSIZE * 0.4f));
	gfx_fill_ring(unconnectedS, (Point){COMPSIZE * 0.2f, COMPSIZE * 0.2f}, COMPSIZE * 0.17f, COMPTHICKNESS, 0x00ff00ff);
	SDL_Texture *unconnected = SDL_CreateTextureFromSurface(renderer, unconnectedS);
	SDL_FreeSurface(unconnectedS);
	for (size_t n = 0; n < vec->count; n++)
	{
		Node *node = nodev_at(vec, n);
		for (int p = 0; p < node->component.funData.inC; p++)
		{
			if (node->component.pinData.pins[p].occupied)
				continue;
			Point pos = component_get_pin_position(&node->component, p);
			SDL_Rect dstRect = {
					(int)(pos.x - COMPSIZE * 0.1f - cameraPos.x),
					(int)(pos.y - COMPSIZE * 0.1f - cameraPos.y),
					(int)(COMPSIZE * 0.2f),
					(int)(COMPSIZE * 0.2f)
			};
			SDL_RenderCopy(renderer, unconnected, NULL, &dstRect);
		}	
	}
	SDL_DestroyTexture(unconnected);
}

void wiredrawing_end(WireDrawing *wireDrawing, NodeVector *vec, Point mousePos) {
	float minDist = INFINITY;
	int minNode = 0;
	int minPin = 0;
	int curNode = 0;
	for (size_t n = 0; n < vec->count; n++) {
		Node *node = nodev_at(vec, n);
		if (node == wireDrawing->origin)
			curNode = n;
		for (size_t p = 0; p < node->component.funData.inC; p++)
		{
			if (node->component.pinData.pins[p].occupied)
				continue;
			Point pin = component_get_pin_position(&node->component, p);
			float dist = (mousePos.x - pin.x) * (mousePos.x - pin.x) + (mousePos.y - pin.y) * (mousePos.y - pin.y);
			if (dist < minDist) {
				minDist = dist;
				minNode = n;
				minPin = p;
			}
		}
	}
	if (minDist < 30 * 30)
		nodev_connect(vec, curNode, wireDrawing->originPin - wireDrawing->origin->component.funData.inC, minNode, minPin);
}