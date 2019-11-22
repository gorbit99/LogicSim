#include "Save.h"
#include "debugmalloc.h"

void save_node(FILE *file, Node *node) {
	fprintf(file, "N %s %f,%f\n", node->component.name, node->component.x, node->component.y);
}

bool save_vector(NodeVector *vector, char *fileName) {
	FILE *file = fopen(fileName, "wt");
	if (file == NULL) {
		log_error("Couldn't open file %s!\n", fileName);
		return false;
	}

	for (size_t n = 0; n < vector->count; n++)
		save_node(file, nodev_at(vector, n));
	for (size_t n = 0; n < vector->count; n++) {
		Node *node = nodev_at(vector, n);
		for (int w = 0; w < node->component.funData.outC; w++) {
			Wire *wire = &node->wires[w];
			for (size_t c = 0; c < wire->conCount; c++) {
				Connection *conn = &wire->connections[c];
				fprintf(file, "C %d %d %d %d\n", n, w, conn->dest, conn->pin);
			}
		}
	}
	fclose(file);
	return true;
}

NodeVector load_vector(char *fileName, TTF_Font *font, SDL_Renderer *renderer) {
	NodeVector result = nodev_create(0);
	FILE *file = fopen(fileName, "rt");
	if (file == NULL) {
		log_error("Couldn't open file %s!\n", fileName);
		return result;
	}
	char name[256];
	float x, y;
	while (fscanf(file, "N %s %f,%f\n", name, &x, &y) == 3)
		nodev_push_back(&result, node_create(name, (Point) {x, y}, font, renderer));
	int n1, p1, n2, p2;
	while (fscanf(file, "C %d %d %d %d\n", &n1, &p1, &n2, &p2) == 4)
		nodev_connect(&result, n1, p1, n2, p2);
	fclose(file);
	return result;
}

typedef struct InnerConnection {
	enum Type {
		WIRE,
		INPUT
	} type;
	int id;
} InnerConnection;

typedef struct Modules {
	int num;
	struct InnerConnection *connections;
} Modules;

bool save_as_module(NodeVector *vector, char *name) {
	char funPath[256];
	sprintf(funPath, "res/%s.fun", name);
	FILE *funFile = fopen(funPath, "wt");
	if (funFile == NULL) {
		log_error("Couldn't open file %s!\n", funPath);
		return false;
	}

	int inC = 0, outC = 0, assignC = 0, wireC = 0;
	for (size_t i = 0; i < vector->count; i++) {
		Node *node = nodev_at(vector, i);
		switch (node->component.type) {
			case CT_SWITCH: {
				inC++;
				break;
			}
			case CT_LED: {
				outC++;
				break;
			}
			case CT_MODULE: {
				assignC++;
				for (int w = 0; w < node->component.funData.outC; w++) {
					wireC += node->wires[w].conCount;
				}
				break;
			}
			default: {
				break;
			}
		}
	}

	Modules *modules = (struct Modules *) malloc(sizeof(struct Modules) * assignC);

	int index = 0;
	for (size_t i = 0; i < vector->count; i++) {
		Node *node = nodev_at(vector, i);
		if (node->component.type == CT_MODULE) {
			modules[index].connections = (struct InnerConnection *) malloc(
					sizeof(struct InnerConnection) * node->component.funData.inC);
			index++;
		}
	}

	for (size_t i = 0; i < vector->count; i++) {
		Node *node = nodev_at(vector, i);
		if (node->component.type == CT_MODULE) {
	}

	fprintf(funFile, "assigns %d\nin %d\nout %d\n", assignC + wireC, inC, outC);

	fclose(funFile);
}