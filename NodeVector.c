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
	for (int i = index + 1; i < vector->count; i++) {
		vector->nodes[i - 1] = vector->nodes[i];
	}
	vector->count--;
}

void nodev_free(NodeVector *vector) {
	for (int i = 0; i < vector->count; i++)
		node_free(&vector->nodes[i]);
	free(vector->nodes);
}
