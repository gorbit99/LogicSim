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
	}
	memcpy(&(vector->nodes[vector->count]), &node, sizeof(node));
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
	node_set_connection(nodev_at(vector, idA), pinA, nodev_at(vector, idB), pinB);
}

void nodev_update(NodeVector *vector) {
	for (int i = 0; i < vector->count; i++) {
		node_run(nodev_at(vector, i));
		node_update_values(nodev_at(vector, i));
	}
}

void nodev_free(NodeVector *vector) {
	for (size_t i = 0; i < vector->count; i++)
		node_free(&vector->nodes[i]);
	free(vector->nodes);
}