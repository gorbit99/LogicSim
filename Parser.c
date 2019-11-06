#include "Parser.h"
#include "debugmalloc.h"

void __parse_assign(Assign *result, char *str) {
    if (str[0] == '%') { //Logic function
        result->outPins = (int *)malloc(sizeof(int));
        char op[512];
        sscanf(str, "%%%d = %[^\n]", result->outPins, op);
        result->op = parser_string_to_op(op);
        result->outNum = 1;
    } else if (str[0] == '{') { //Module
        int numOfOut = 1;
        for (int i = 0; str[i] != '}'; i++)
            if (str[i] == ',') numOfOut++;
        result->outPins = (int *)malloc(sizeof(int) * numOfOut);
        result->outPins[0] = 0;
        result->outNum = numOfOut;
        int read;
        int offset = 1;
        for (int i = 0; i < numOfOut; i++) {
            int endRead = 0;
            sscanf(str + offset, "%%%d%n, %n", &result->outPins[i], &read, &endRead);
            if (endRead == 0)
                offset += read + 1;
            else
                offset += endRead;
        }
        sscanf(str + offset, " = %n", &read);
        offset += read;
        result->op = parser_string_to_module(str + offset);
    }
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
            bool *moduleIn = (bool *)malloc(sizeof(bool) * op->A.module->function.inC);
            for (int i = 0; i < op->A.module->function.inC; i++)
                moduleIn[i] = in[op->A.module->inPins[i]];
            parser_run_function(&op->A.module->function, moduleIn, out);
            break;
        }
    }
}

void parser_run_assign(Assign *assign, bool *in, bool *out) {
    if (assign->op->type == OP_MODULE)
        parser_handle_operation(assign->op, in, out);
    else
        parser_handle_operation(assign->op, in, &out[assign->outPins[0]]);
}

void parser_run_function(FunctionData *function, bool *in, bool *out) {
    for (int i = 0; i < function->assignC; i++)
        parser_run_assign(&function->assigns[i], in, out);
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
			default:
				break;
        }
    }
    return opStack[0];
}

Operation *parser_string_to_module(char *str) {
    char name[256];
    int readPos = 0;
    sscanf(str, "%[A-Za-z0-9-_.]%n", name, &readPos);
    char path[256];
    sprintf(path, "res/%s.fun", name);
    Operation *op = (Operation *)malloc(sizeof(Operation));
    op->type = OP_MODULE;
    op->A.module = (Module *)malloc(sizeof(Module));
    op->A.module->function = parser_load_function(path);
    readPos++;
    op->A.module->inPins = (int *)malloc(sizeof(int) * op->A.module->function.inC);
    for (int i = 0; i < op->A.module->function.inC; i++) {
        int read;
        sscanf(str + readPos, "$%d, %n", &op->A.module->inPins[i], &read);
        readPos += read;
    }
    return op;
}

void parser_free_operation(Operation *operation) {
    if (operation->type == OP_CONST || operation->type == OP_PIN) {
        free(operation);
        return;
    }
    else if (operation->type == OP_NOT) {
        parser_free_operation(operation->A.op);
        free(operation);
        return;
    }
    else if (operation->type == OP_MODULE) {
        free(operation->A.module->inPins);
        free(operation->A.module);
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