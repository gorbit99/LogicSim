#include "Save.h"
#include "debugmalloc.h"

void save_node(FILE *file, Node *node) {
	fprintf(file, "N %f,%f %s\n", node->component.x, node->component.y, node->component.name);
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
	while (fscanf(file, "N %f,%f %[^\n]\n", &x, &y, name) == 3)
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
	sprintf(funPath, "res/Modules/%s.fun", name);
	FILE *funFile = fopen(funPath, "wt");
	if (funFile == NULL) {
		log_error("Couldn't open file %s!\n", funPath);
		return false;
	}

    Modules *modules = (Modules *)malloc(sizeof(Module) * vector->count);
    for (size_t n = 0; n < vector->count; n++) {
        modules[n].connections = NULL;
    }

	int inC = 0, outC = 0, assignC = 0, wireC = 0;
    for (size_t n = 0; n < vector->count; n++) {
        Node *node = nodev_at(vector, n);
        switch (node->component.type)
        {
        case CT_SWITCH:
            inC++;
            break;
        case CT_LED:
            outC++;
            assignC++;
            break;
        case CT_MODULE:
            assignC++;
            wireC += node->component.funData.outC;
            break;
        default:
            break;
        }
        modules[n].num = node->component.funData.inC;
        modules[n].connections = (InnerConnection *)malloc(sizeof(InnerConnection) * modules[n].num);    
    }

    fprintf(funFile, "assigns %d\n", assignC);
    fprintf(funFile, "in %d\n", inC);
    fprintf(funFile, "out %d\n", outC);
    fprintf(funFile, "wires %d\n", wireC);

    int wireIndex = 0, inputIndex = 0;
    for (size_t n = 0; n < vector->count; n++) {
        Node *node = nodev_at(vector, n);
        for (int w = 0; w < node->component.funData.outC; w++) {
            Wire *wire = &node->wires[w];
            for (size_t c = 0; c < wire->conCount; c++) {
                Connection *conn = &wire->connections[c];
                switch (node->component.type)
                {
                case CT_SWITCH:
                    modules[conn->dest].connections[conn->pin].id = inputIndex;
                    modules[conn->dest].connections[conn->pin].type = INPUT;
                    break;
                case CT_MODULE:
                    modules[conn->dest].connections[conn->pin].id = wireIndex;
                    modules[conn->dest].connections[conn->pin].type = WIRE;
                    break;
                default:
                    break;
                }
            }
            if (node->component.type == CT_MODULE)
                wireIndex++;
        }
        if (node->component.type == CT_SWITCH)
            inputIndex++;
    }
    
    wireIndex = 0;
    int outputIndex = 0;
    for (size_t n = 0; n < vector->count; n++) {
        Node *node = nodev_at(vector, n);
        if (node->component.type == CT_MODULE) {
            fprintf(funFile, "{");
            bool first = true;
            for (int w = 0; w < node->component.funData.outC; w++) {
                if (!first)
                    fprintf(funFile, ", ");
                else
                    first = false;
                fprintf(funFile, "W%d", wireIndex);
                wireIndex++;
            }
            fprintf(funFile, "} = %s(", node->component.name);
            first = true;
            for (int i = 0; i < modules[n].num; i++) {
                if (!first)
                    fprintf(funFile, ", ");
                else
                    first = false;
                switch (modules[n].connections[i].type)
                {
                case WIRE:
                    fprintf(funFile, "W");
                    break;
                case INPUT:
                    fprintf(funFile, "$");
                    break;
                }
                fprintf(funFile, "%d", modules[n].connections[i].id);
            }
            fprintf(funFile, ")\n");
        } else if (node->component.type == CT_LED) {
            fprintf(funFile, "%%%d = W%d\n", outputIndex, modules[n].connections[0].id);
            outputIndex++;
        }
    }

    for (size_t n = 0; n < vector->count; n++) {
        free(modules[n].connections);
    }
    free(modules);

	fclose(funFile);

    char cmpPath[256];
    sprintf(cmpPath, "res/Modules/%s.cmp", name);
    FILE *cmpFile = fopen(cmpPath, "wt");
    if (cmpFile == NULL) {
        log_error("Couldn't open file %s!\n", cmpPath);
        return false;
    }
    int maxPin = max(inC, outC);
    float w = (float)strlen(name) * 0.2f, h = (float)(maxPin + 1) * 0.5f;

    char *upperName = (char *)malloc(sizeof(char) * (strlen(name) + 1));
    int i;
    for (i = 0; name[i] != '\0'; i++)
        upperName[i] = (char)toupper(name[i]);
    upperName[i] = '\0';

    fprintf(cmpFile, "%f %f\n", w, h);
    fprintf(cmpFile, "L %f,%f %f,%f\n", 0.05f, 0.05f, w - 0.1f, 0.05f);
    fprintf(cmpFile, "L %f,%f %f,%f\n", 0.05f, 0.05f, 0.05f, h - 0.1f);
    fprintf(cmpFile, "L %f,%f %f,%f\n", 0.05f, h - 0.1f, w - 0.1f, h - 0.1f);
    fprintf(cmpFile, "L %f,%f %f,%f\n", w - 0.1f, 0.05f, w - 0.1f, h - 0.1f);
    fprintf(cmpFile, "TXT %f,%f %s\n", w / 2, h / 2, upperName);

    free(upperName);

    fclose(cmpFile);

    char datPath[256];
    sprintf(datPath, "res/Modules/%s.dat", name);

    FILE *datFile = fopen(datPath, "wt");
    if (datFile == NULL) {
    	log_error("Couldn't open file %s!\n", datPath);
    	return false;
    }

    fprintf(datFile, "%d\n", inC + outC);
    for (int in = 0; in < inC; in++) {
    	fprintf(datFile, "in I%d %f,%f 3.1415\n", in, 0.05f, h / (float)(inC + 1) * (float)in + 0.5f);
    }
    for (int out = 0; out < outC; out++) {
		fprintf(datFile, "out O%d %f,%f 0\n", out, w - 0.1f, h / (float)(outC + 1) * (float)out + 0.5f);
	}

    fclose(datFile);
}