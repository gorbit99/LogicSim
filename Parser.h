#ifndef HOMEWORK_PARSER_H
#define HOMEWORK_PARSER_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "Utility.h"
#include <string.h>

typedef enum OperationType {
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_NOT,
    OP_PIN,
    OP_CONST,
    OP_MODULE
} OperationType;

struct Module;

typedef struct Operation {
    OperationType type;
    union A {
        struct Operation *op;
        int pin;
        bool c;
        struct Module *module;
    } A;
    struct Operation *B;
} Operation;

typedef struct Assign {
    int *outPins;
    Operation op;
} Assign;

typedef struct FunctionData {
    int inC, outC;
    Assign *assigns;
} FunctionData;

typedef struct Module {
    FunctionData function;
} Module;

char *parser_load_function(char *path);

void parser_handle_operation(Operation *op, bool *in, bool *out);

Operation parser_string_to_op(char *str);

#endif //HOMEWORK_PARSER_H