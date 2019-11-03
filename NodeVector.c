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

void nodev_recalc_levels(NodeVector *vector) {
	for (size_t i = 0; i < vector->count; i++)
		nodev_at(vector, i)->level = 0;
	for (size_t i = 0; i < vector->count; i++)
		if (nodev_at(vector, i)->component.type == CT_SWITCH)
			node_update_level(nodev_at(vector, i), 1);
}

void nodev_switch(NodeVector *vector, int id) {
	if (nodev_at(vector, id)->component.type != CT_SWITCH)
		return;
	EventQueue queue = evqueue_create();

	nodev_at(vector, id)->outValues[0] ^= 1;
	evqueue_push(&queue, (Event){nodev_at(vector, id)->connections[0].other});
	node_set_inval(nodev_at(vector, id)->connections[0].other, 
				nodev_at(vector, id)->connections[0].pinB, 
				nodev_at(vector, id)->outValues[0]);

	while(queue.count != 0) {
		Event e = evqueue_pop(&queue);
		if (node_handle(e.node)) {
			for (int i = 0; i < e.node->component.funData.outC; i++)
				evqueue_push(&queue, (Event){e.node->connections[i].other});
		}
	}

	evqueue_free(&queue);
}

void nodev_update_all(NodeVector *vector) {
	for (size_t i = 0; i < vector->count; i++)
		node_handle(nodev_at(vector, i));
}

void nodev_free(NodeVector *vector) {
	for (size_t i = 0; i < vector->count; i++)
		node_free(&vector->nodes[i]);
	free(vector->nodes);
}