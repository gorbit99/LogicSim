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
		for (int i = 0; i < vector->count; i++)
			for (int j = 0; j < vector->nodes[i].component.funData.outC; j++)
				vector->nodes[i].wires[j].origin = &vector->nodes[i];
	}
	vector->nodes[vector->count] = node;
	for (int i = 0; i < vector->nodes[vector->count].component.funData.outC; i++)
		vector->nodes[vector->count].wires[i].origin = &vector->nodes[vector->count];
	vector->count++;
}

void nodev_erase(NodeVector *vector, int index) {
	for (size_t i = index + 1; i < vector->count; i++) {
		vector->nodes[i - 1] = vector->nodes[i];
	}
	vector->count--;
}

Node *nodev_at(NodeVector *vector, int index) {
	return &vector->nodes[index];
}

void nodev_connect(NodeVector *vector, int idA, int pinA, int idB, int pinB) {
	node_add_connection(nodev_at(vector, idA), pinA, nodev_at(vector, idB), pinB);
}

void nodev_render(NodeVector *vector, Point camPos) {
	for (size_t i = 0; i < vector->count; i++) {
		node_render(nodev_at(vector, i), camPos);
	}
}

void nodev_update(NodeVector *vector) {
	for (size_t i = 0; i < vector->count; i++) {
		node_run(nodev_at(vector, i));
		node_update_values(nodev_at(vector, i));
	}
}

void nodev_check_clicks(NodeVector *vector, Point mousePos) {
	for (size_t i = 0; i < vector->count; i++) {
		if (nodev_at(vector, i)->component.type == CT_SWITCH)
			if (node_is_over(nodev_at(vector, i), mousePos)) {
				nodev_at(vector, i)->outValues[0] ^= 1u;
				for (size_t w = 0; w < nodev_at(vector, i)->wires[0].conCount; w++)
					node_set_inval(nodev_at(vector, i)->wires[0].connections[w].dest,
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