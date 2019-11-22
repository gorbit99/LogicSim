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
        nodev_push_back(&result, node_create(name, (Point){x, y}, font, renderer));
    int n1, p1, n2, p2;
    while (fscanf(file, "C %d %d %d %d\n", &n1, &p1, &n2, &p2) == 4)
        nodev_connect(&result, n1, p1, n2, p2);
    fclose(file);
    return result;
}

bool save_as_module(NodeVector *vector, char *fileName) {
    
}