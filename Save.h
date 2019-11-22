#ifndef HOMEWORK_SAVE_H
#define HOMEWORK_SAVE_H

#include <stdio.h>
#include <stdbool.h>
#include "NodeVector.h"
#include "Utility.h"

void save_node(FILE *file, Node *node);

void save_connections(FILE *file, Node *node);

bool save_vector(NodeVector *vector, char *fileName);

NodeVector load_vector(char *fileName, TTF_Font *font, SDL_Renderer *renderer);

bool save_as_module(NodeVector *vector, char *fileName);

#endif