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

void nodev_free(NodeVector *vector) {
	for (size_t i = 0; i < vector->count; i++)
		node_free(&vector->nodes[i]);
	free(vector->nodes);
}

void nodev_update(NodeVector *vector) {
	for (size_t i = 0; i < vector->count; i++) {
		for (int j = 0; j < vector->nodes[i].component.funData.inC; j++)
			vector->nodes[i].prevInValues[j] = vector->nodes[i].inValues[j];
		vector->nodes[i].prevDirty = vector->nodes[i].dirty;
	}

	bool updated;
	do {
		updated = false;
		for (int k = 0; k < 2; k++) {
			for (size_t i = 0; i < vector->count; i++) {
				if (vector->nodes[i].prevDirty) {
					node_update(&vector->nodes[i]);
					updated = true;
				}
			}
		}
		for (size_t i = 0; i < vector->count; i++) {
			for (int j = 0; j < vector->nodes[i].component.funData.inC; j++)
				vector->nodes[i].prevInValues[j] = vector->nodes[i].inValues[j];
			vector->nodes[i].prevDirty = vector->nodes[i].dirty;
			vector->nodes[i].dirty = false;
		}
	} while (updated);
}
