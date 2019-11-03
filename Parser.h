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
    OP_MODULE,
    OP_LPAREN
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
    Operation *op;
} Assign;

typedef struct FunctionData {
    int inC, outC, assignC;
    Assign *assigns;
} FunctionData;

typedef struct Module {
    FunctionData function;
} Module;

FunctionData parser_load_function(char *path);

void parser_handle_operation(Operation *op, bool *in, bool *out);

void parser_run_assign(Assign *assign, bool *in, bool *out);

void parser_run_function(FunctionData *function, bool *in, bool *out);

Operation *parser_string_to_op(char *str);

void parser_free_operation(Operation *operation);

void parser_free_assign(Assign *assign);

void parser_free_function(FunctionData *functionData);

#endif //HOMEWORK_PARSER_H