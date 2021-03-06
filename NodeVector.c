#include "NodeVector.h"
#include "debugmalloc.h"

NodeVector nodev_create(size_t count) {
	NodeVector result;
	result.capacity = count * 2 + 1;
	result.count = count;
	result.nodes = (Node *)malloc(sizeof(Node) * result.capacity);
	return result;
}

void nodev_push_back(NodeVector *vector, Node node) {
	if (vector->count >= vector->capacity) {
		vector->capacity *= 2;
		Node *newmem = (Node *)realloc(vector->nodes, sizeof(Node) * vector->capacity);
		if (newmem == NULL) {
			log_error("Couldn't reallocate vector!\n");
			return;
		}
		vector->nodes = newmem;
		for (size_t i = 0; i < vector->count; i++)
			for (int j = 0; j < vector->nodes[i].component.funData.outC; j++)
				vector->nodes[i].wires[j].origin = &vector->nodes[i];
	}
	vector->nodes[vector->count] = node;
	for (int i = 0; i < vector->nodes[vector->count].component.funData.outC; i++)
		vector->nodes[vector->count].wires[i].origin = &vector->nodes[vector->count];
	vector->count++;
}

void nodev_erase(NodeVector *vector, int index) {
	node_free(nodev_at(vector, index));
	for (size_t i = 0; i < vector->count; i++) {
		if (i == index)
			continue;
		Node *node = nodev_at(vector, i);
		for (int w = 0; w < node->component.funData.outC; w++) {
			Wire *wire = &node->wires[w];
			for (size_t c = 0; c < wire->conCount; c++) {
				Connection *conn = &wire->connections[c];
				if (conn->dest > index)
					conn->dest--;
			}
		}
	}
	for (size_t i = index + 1; i < vector->count; i++) {
		vector->nodes[i - 1] = vector->nodes[i];
		for (int j = 0; j < vector->nodes[i].component.funData.outC; j++)
				vector->nodes[i - 1].wires[j].origin = &vector->nodes[i - 1];
	}
	vector->count--;
}

Node *nodev_at(NodeVector *vector, int index) {
	return &vector->nodes[index];
}

void nodev_connect(NodeVector *vector, int idA, int pinA, int idB, int pinB) {
	node_add_connection(nodev_at(vector, idA), pinA, idB, pinB, vector->nodes);
}

void nodev_render(NodeVector *vector, Point camPos) {
	for (size_t i = 0; i < vector->count; i++) {
		node_render(nodev_at(vector, i), camPos);
	}
}

void nodev_update(NodeVector *vector) {
	for (size_t i = 0; i < vector->count; i++) {
		node_run(nodev_at(vector, i), vector->nodes);
		node_update_values(nodev_at(vector, i));
	}
}

void nodev_check_clicks(NodeVector *vector, Point mousePos) {
	for (size_t i = 0; i < vector->count; i++) {
		if (nodev_at(vector, i)->component.type == CT_SWITCH)
			if (node_is_over(nodev_at(vector, i), mousePos)) {
				nodev_at(vector, i)->outValues[0] ^= 1u;
				for (size_t w = 0; w < nodev_at(vector, i)->wires[0].conCount; w++)
					node_set_inval(nodev_at(vector,nodev_at(vector, i)->wires[0].connections[w].dest),
						nodev_at(vector, i)->wires[0].connections[w].pin,
						nodev_at(vector, i)->outValues[0]);
			}
	}
}

void nodev_free(NodeVector *vector) {
	for (size_t i = 0; i < vector->count; i++)
		node_free(&vector->nodes[i]);
	free(vector->nodes);
}

void nodev_reposition(NodeVector *vector, Node *node, Point position) {
	if (node->component.x != position.x || node->component.y != position.y) {
		node->component.x = position.x;
		node->component.y = position.y;
		node_reposition_wires(node, vector->nodes);
		for (size_t i = 0; i < vector->count; i++) {
			Node *origin = nodev_at(vector, i);
			for (int w = 0; w < origin->component.funData.outC; w++) {
				Wire *wire = &origin->wires[w];
				for (size_t c = 0; c < wire->conCount; c++) {
					Connection *con = &wire->connections[c];
					if (nodev_at(vector, con->dest) == node) {
						connection_reposition(con, origin, w, node);
					}
				}
			}
		}
	}
}

void nodev_delete(NodeVector *vector, Node *node) {
	for (int w = 0; w < node->component.funData.outC; w++) {
		Wire *wire = &node->wires[w];
		for (size_t c = 0; c < wire->conCount; c++) {
			Connection *conn = &wire->connections[c];
			nodev_at(vector, conn->dest)->component.pinData.pins[conn->pin].occupied = false;
		}
	}
	for (size_t i = 0; i < vector->count; i++) {
		if (nodev_at(vector, i) == node) {
			for (size_t n = 0; n < vector->count; n++) {
				if (n == i)
					continue;
				Node *other = nodev_at(vector, n);
				for (int w = 0; w < other->component.funData.outC; w++) {
					Wire *wire = &other->wires[w];
					for (size_t c = 0; c < wire->conCount; c++) {
						Connection *conn = &wire->connections[c];
						if (conn->dest == i) {
							wire_erase(wire, c);
							c--;
						}
					}
				}
			}
			nodev_erase(vector, i);
			break;
		}
	}
}