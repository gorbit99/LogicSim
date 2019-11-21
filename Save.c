#include "Save.h"
#include "debugmalloc.h"

void save_node(FILE *file, Node *node) {
    fprintf(file, "\t%s %f,%f\n", node->component.name, node->component.x, node->component.y);
}

bool save_vector(NodeVector *vector, char *fileName) {
    FILE *file = fopen(fileName, "wt");
    if (file == NULL) {
        log_error("Couldn't open file %s!", fileName);
        return false;
    }

    fprintf(file, "Nodes\n");
    for (size_t n = 0; n < vector->count; n++)
        save_node(file, nodev_at(vector, n));
    fprintf(file, "Connections\n");
    for (size_t n = 0; n < vector->count; n++) {
        Node *node = nodev_at(vector, n);
        for (int w = 0; w < node->component.funData.outC; w++) {
            Wire *wire = &node->wires[w];
            for (size_t c = 0; c < wire->conCount; c++) {
                Connection *conn = &wire->connections[c];
                fprintf(file, "\t%d %d %d %d\n", n, w, conn->dest, conn->pin);
            }
        }
    }
}