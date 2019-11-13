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
	Point pin = component_get_pin_position(&node->component, pinA);
	return sqrtf((pin.x - P.x) * (pin.x - P.x) + (pin.y - P.y) * (pin.y - P.y));
}

Point closest_point_on_wires(NodeVector *vec, float *dist, Point P) {
	Point closest = {};
	*dist = INFINITY;
	for (size_t n = 0; n < vec->count; n++) {
		Node *node = nodev_at(vec, n);
		for (int w = 0; w < node->component.funData.outC; w++) {
			Wire *wire = &node->wires[w];
			for (size_t c = 0; c < wire->conCount; c++) {
				Connection *conn = &wire->connections[c];
				Node *dest = nodev_at(vec, conn->dest);
				float curDist;
				Point curClosest = closest_on_wire(node, wire->originPin, dest, conn->pin, P, &curDist);
				if (curDist < *dist) {
					closest = curClosest;
					*dist = curDist;
				}
			}
		}
	}
	return closest;
}
