#ifndef HOMEWORK_CONFIGHANDLER_H
#define HOMEWORK_CONFIGHANDLER_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "Utility.h"

void config_init();

bool config_get_bool(char *field);

int config_get_int(char *field);

char *config_get_string(char *field);

void config_set_bool(char *field, bool value);

void config_set_int(char *field, int value);

void config_set_string(char *field, char *value);

void config_save();

void config_free();

#endif //HOMEWORK_CONFIGHANDLER_H