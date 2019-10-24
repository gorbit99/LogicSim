#include "Parser.h"
#include "debugmalloc.h"

void __parse_assign(Assign *result, char *str) {
    result->outPins = (int *)malloc(sizeof(int));
    char op[512];
    sscanf(str, "%%%d = %[^\n]", result->outPins, op);
    result->op = parser_string_to_op(op);
}

FunctionData parser_load_function(char *path) {
    FILE *f = fopen(path, "r");
    
    if (f == NULL) {
        log_error("Couldn't open file: %s\n", path);
        return (FunctionData){};
    }

    int inC, outC, assignC;
    fscanf(f, "assigns %d\nin %d\nout %d\n", &assignC, &inC, &outC);

    FunctionData function;
    function.assigns = (Assign *)malloc(sizeof(Assign) * assignC);
    function.inC = inC;
    function.outC = outC;
    function.assignC = assignC;

    for (int i = 0; i < assignC; i++) {
        char assign[512];
        fscanf(f, "%[^\n]\n", assign);
        __parse_assign(&function.assigns[i], assign);
    }

    fclose(f);
    return function;
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

Operation *parser_string_to_op(char *str) {
    int sp = 0;
    Operation stack[512];
    int qp = 0;
    Operation queue[512];

    while (*str != '\0') {
        if (*str == ' ') {
            str++;
            continue;
        }
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
        } else if (*str == '*' || *str == '&') {
            while (sp > 0 && (stack[sp - 1].type == OP_AND || stack[sp - 1].type == OP_OR || stack[sp - 1].type == OP_XOR)) {
                sp--;
                queue[qp] = stack[sp];
                qp++;
            }
            stack[sp].type = OP_AND;
            sp++;
        } else if (*str == '+' || *str == '|') {
            while (sp > 0 && (stack[sp - 1].type == OP_AND || stack[sp - 1].type == OP_OR || stack[sp - 1].type == OP_XOR)) {
                sp--;
                queue[qp] = stack[sp];
                qp++;
            }
            stack[sp].type = OP_OR;
            sp++;
        } else if (*str == '(') {
            stack[sp].type = OP_LPAREN;
            sp++;
        } else if (*str == ')') {
            while (stack[sp - 1].type != OP_LPAREN) {
                sp--;
                queue[qp] = stack[sp];
                qp++;
            }
            sp--;
        } else if (*str == '^') {
            while (sp > 0 && (stack[sp - 1].type == OP_AND || stack[sp - 1].type == OP_OR || stack[sp - 1].type == OP_XOR)) {
                sp--;
                queue[qp] = stack[sp];
                qp++;
            }
            stack[sp].type = OP_XOR;
            sp++;
        } else if (*str == '/' || *str == '!' || *str == '~') {
            while (sp > 0 && stack[sp - 1].type == OP_NOT) {
                sp--;
                queue[qp] = stack[sp];
                qp++;
            }
            stack[sp].type = OP_NOT;
            sp++;
        }
        str++;
    }
    int queueLen = qp + sp;
    while (sp > 0) {
        sp--;
        queue[qp] = stack[sp];
        qp++;
    }

    Operation *opStack[256];
    sp = 0;

    for (qp = 0; qp < queueLen; qp++) {
        switch (queue[qp].type) {
            case OP_PIN:
            case OP_CONST:
                opStack[sp] = (Operation *)malloc(sizeof(Operation));
                opStack[sp]->type = queue[qp].type;
                opStack[sp]->A = queue[qp].A;
                sp++;
                break;
            case OP_AND:
            case OP_OR:
            case OP_XOR: {
                Operation *op = (Operation *)malloc(sizeof(Operation));
                op->type = queue[qp].type;
                op->A.op = opStack[sp - 2];
                op->B = opStack[sp - 1];
                opStack[sp - 2] = op;
                sp--;
                break;
            }
            case OP_NOT: {
                Operation *op = (Operation *)malloc(sizeof(Operation));
                op->type = OP_NOT;
                op->A.op = opStack[sp - 1];
                opStack[sp - 1] = op;
                break;
            }
        }
    }
    return opStack[0];
}

void parser_free_operation(Operation *operation) {
    if (operation->type == OP_CONST || operation->type == OP_PIN) {
        free(operation);
        return;
    }
    if (operation->type == OP_NOT) {
        parser_free_operation(operation->A.op);
        free(operation);
        return;
    }
    parser_free_operation(operation->A.op);
    parser_free_operation(operation->B);
    free(operation);
}

void parser_free_assign(Assign *assign) {
    parser_free_operation(assign->op);
    free(assign->outPins);
}

void parser_free_function(FunctionData *functionData) {
    for (int i = 0; i < functionData->assignC; i++)
        parser_free_assign(&functionData->assigns[i]);
    free(functionData->assigns);
}