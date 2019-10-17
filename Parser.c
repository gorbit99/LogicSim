#include "Parser.h"
#include "debugmalloc.h"

char *parser_load_function(char *path) {
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        log_error("Couldn't open file!");
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *data = (char *)malloc(sizeof(char) * len);
    fread(data, sizeof(char), len, f);
    return data;
}

void parser_handle_operation(Operation *op, bool *in, bool *out) {
    switch (op->type) {
        case OP_CONST:
            out[0] = op->A.c;
            return;
        case OP_PIN:
            out[0] = in[op->A.pin];
            return;
        case OP_AND: {
            bool params[2];
            parser_handle_operation(op->A.op, in, params);
            parser_handle_operation(op->B, in, params + 1);
            out[0] = params[0] && params[1];
            break;
        }
        case OP_OR: {
            bool params[2];
            parser_handle_operation(op->A.op, in, params);
            parser_handle_operation(op->B, in, params + 1);
            out[0] = params[0] || params[1];
            break;
        }
        case OP_XOR: {
            bool params[2];
            parser_handle_operation(op->A.op, in, params);
            parser_handle_operation(op->B, in, params + 1);
            out[0] = params[0] ^ params[1];
            break;
        }
        case OP_NOT: {
            bool params[1];
            parser_handle_operation(op->A.op, in, params);
            out[0] = !params[0];
            break;
        }
        case OP_MODULE: {
            //TODO
            break;
        }
    }
}

Operation parser_string_to_op(char *str) {
    int sp = 0;
    Operation stack[512];
    int qp = 0;
    Operation queue[512];

    while (*str != '\0') {
        if (*str == '0' || *str == '1') {
            queue[qp].type = OP_CONST;
            queue[qp].A.c = *str == '1';
            qp++;
        } else if (*str == '$') {
            int pinNum;
            int numBytes;
            sscanf(str + 1, "%d%n", &pinNum, &numBytes);
            queue[qp].type = OP_PIN;
            queue[qp].A.pin = pinNum;
            qp++;
            str += numBytes;
        } else if (*str == '/') {
            queue[qp].type = OP_NOT;
        }
    }
}