#include "WireDrawing.h"
#include "debugmalloc.h"

float dist_from_wire(Node *nodeA, int pinA, Node *nodeB, int pinB, Point P) {
	Point pinPos1 = component_get_pin_position(&nodeA->component, pinA);
	Point pinPos2 = component_get_pin_position(&nodeB->component, pinB);

}
