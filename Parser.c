#include "Parser.h"
#include "debugmalloc.h"

void __parse_assign(Assign *result, char *str) {
	if (str[0] == '%' || str[0] == 'W') { //Logic function
		result->pinNum = 1;
		result->pins = (AssignPin *) malloc(sizeof(AssignPin));
		result->pins[0].type = str[0] == '%' ? AP_PIN : AP_WIRE;
		char *operation = (char *) malloc(sizeof(char) * strlen(str));
		sscanf(str + 1, "%d = %[^\n]", &result->pins[0].pinId, operation);
		result->op = parser_string_to_op(operation);
		free(operation);
	} else if (str[0] == '{') { //Module
		result->pinNum = 1;
		for (int i = 0; str[i] != '}'; i++)
			if (str[i] == ',') result->pinNum++;
		result->pins = (AssignPin *) malloc(sizeof(AssignPin) * result->pinNum);
		int offset = 1;
		int read;
		for (int i = 0; i < result->pinNum; i++) {
			char pin[256];
			sscanf(str + offset, "%[^,}]%*[,}]%n", pin, &read);
			offset += read + 1;
			switch (pin[0]) {
				case 'X':
				case 'x':
					result->pins[i].type = AP_DONTCARE;
					break;
				case '%': {
					result->pins[i].type = AP_PIN;
					sscanf(pin + 1, "%d", &result->pins[i].pinId);
					break;
				}
				case 'W': {
					result->pins[i].type = AP_WIRE;
					sscanf(pin + 1, "%d", &result->pins[i].pinId);
					break;
				}
			}
		}
		sscanf(str + offset, " = %n", &read);
		offset += read;
		char *operation = (char *) malloc(sizeof(char) * strlen(str));
		strcpy(operation, str + offset);
		result->op = parser_string_to_module(operation);
		free(operation);
	}
}

FunctionData parser_load_function(char *path) {
	FILE *f = fopen(path, "r");

	if (f == NULL) {
		log_error("Couldn't open file: %s\n", path);
		return (FunctionData) {};
	}

	int inC, outC, assignC, wireC;
	fscanf(f, "assigns %d\nin %d\nout %d\n wires %d\n", &assignC, &inC, &outC, &wireC);

	FunctionData function;
	function.assigns = (Assign *) malloc(sizeof(Assign) * assignC);
	function.wires = (bool *) calloc(wireC, sizeof(bool));
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

void parser_handle_operation(Operation *op, const bool *in, bool *wires, bool *out, AssignPin *pins) {
	switch (op->type) {
		case OP_AND: {
			bool params[2];
			AssignPin paramPins[2] = {
					(AssignPin) {AP_PIN, 0},
					(AssignPin) {AP_PIN, 1}
			};
			parser_handle_operation(op->A.op, in, wires, params, paramPins);
			parser_handle_operation(op->B, in, wires, params, paramPins + 1);
			out[pins[0].pinId] = params[0] && params[1];
			break;
		}
		case OP_OR: {
			bool params[2];
			AssignPin paramPins[2] = {
					(AssignPin) {AP_PIN, 0},
					(AssignPin) {AP_PIN, 1}
			};
			parser_handle_operation(op->A.op, in, wires, params, paramPins);
			parser_handle_operation(op->B, in, wires, params, paramPins + 1);
			out[pins[0].pinId] = params[0] || params[1];
			break;
		}
		case OP_XOR: {
			bool params[2];
			AssignPin paramPins[2] = {
					(AssignPin) {AP_PIN, 0},
					(AssignPin) {AP_PIN, 1}
			};
			parser_handle_operation(op->A.op, in, wires, params, paramPins);
			parser_handle_operation(op->B, in, wires, params, paramPins + 1);
			out[pins[0].pinId] = params[0] != params[1];
			break;
		}
		case OP_NOT: {
			bool param;
			AssignPin paramPin = {AP_PIN, 0};
			parser_handle_operation(op->A.op, in, wires, &param, &paramPin);
			out[pins[0].pinId] = !param;
			break;
		}
		case OP_PIN:
			out[pins[0].pinId] = in[op->A.pin];
			break;
		case OP_WIRE:
			out[pins[0].pinId] = wires[op->A.pin];
			break;
		case OP_CONST:
			out[pins[0].pinId] = op->A.c;
			break;
		case OP_MODULE: {
			bool *moduleIn = (bool *) malloc(sizeof(bool) * op->A.module->function.inC);
			bool *moduleOut = (bool *) malloc(sizeof(bool) * op->A.module->function.outC);
			for (int i = 0; i < op->A.module->function.inC; i++) {
				if (op->A.module->inPins[i].type == MP_PIN)
					moduleIn[i] = in[op->A.module->inPins[i].pinId];
				else if (op->A.module->inPins[i].type == MP_WIRE)
					moduleIn[i] = wires[op->A.module->inPins[i].pinId];
				else if (op->A.module->inPins[i].type == MP_PULLUP)
					moduleIn[i] = true;
				else if (op->A.module->inPins[i].type == MP_PULLDOWN)
					moduleIn[i] = false;
			}
			parser_run_function(&op->A.module->function, moduleIn, moduleOut);
			for (int i = 0; i < op->A.module->function.outC; i++) {
				if (pins[i].type == AP_PIN)
					out[pins[i].pinId] = moduleOut[i];
				if (pins[i].type == AP_WIRE)
					wires[pins[i].pinId] = moduleOut[i];
			}
			free(moduleIn);
			free(moduleOut);
			break;
		}
		case OP_LPAREN:
			break;
	}
}

void parser_run_assign(Assign *assign, bool *in, bool *wires, bool *out) {
	parser_handle_operation(assign->op, in, wires, out, assign->pins);
}

void parser_run_function(FunctionData *function, bool *in, bool *out) {
	for (int i = 0; i < function->assignC; i++)
		parser_run_assign(&function->assigns[i], in, function->wires, out);
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
		} else if (*str == 'W') {
			int wireNum;
			int numBytes;
			sscanf(str + 1, "%d%n", &wireNum, &numBytes);
			queue[qp].type = OP_WIRE;
			queue[qp].A.pin = wireNum;
			qp++;
			str += numBytes;
		} else if (*str == '*' || *str == '&') {
			while (sp > 0 &&
			       (stack[sp - 1].type == OP_AND || stack[sp - 1].type == OP_OR || stack[sp - 1].type == OP_XOR)) {
				sp--;
				queue[qp] = stack[sp];
				qp++;
			}
			stack[sp].type = OP_AND;
			sp++;
		} else if (*str == '+' || *str == '|') {
			while (sp > 0 &&
			       (stack[sp - 1].type == OP_AND || stack[sp - 1].type == OP_OR || stack[sp - 1].type == OP_XOR)) {
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
			while (sp > 0 &&
			       (stack[sp - 1].type == OP_AND || stack[sp - 1].type == OP_OR || stack[sp - 1].type == OP_XOR)) {
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
			case OP_WIRE:
			case OP_CONST:
				opStack[sp] = (Operation *) malloc(sizeof(Operation));
				opStack[sp]->type = queue[qp].type;
				opStack[sp]->A = queue[qp].A;
				sp++;
				break;
			case OP_AND:
			case OP_OR:
			case OP_XOR: {
				Operation *op = (Operation *) malloc(sizeof(Operation));
				op->type = queue[qp].type;
				op->A.op = opStack[sp - 2];
				op->B = opStack[sp - 1];
				opStack[sp - 2] = op;
				sp--;
				break;
			}
			case OP_NOT: {
				Operation *op = (Operation *) malloc(sizeof(Operation));
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
	sprintf(path, "res/Modules/%s.fun", name);
	Operation *op = (Operation *) malloc(sizeof(Operation));
	op->type = OP_MODULE;
	op->A.module = (Module *) malloc(sizeof(Module));
	op->A.module->function = parser_load_function(path);
	readPos++;
	op->A.module->inPins = (ModulePin *) malloc(sizeof(ModulePin) * op->A.module->function.inC);
	for (int i = 0; i < op->A.module->function.inC; i++) {
		int read;
		if (str[readPos] == '$')
			op->A.module->inPins[i].type = MP_PIN;
		else if (str[readPos] == 'W')
			op->A.module->inPins[i].type = MP_WIRE;
		else if (str[readPos] == '0')
			op->A.module->inPins[i].type = MP_PULLDOWN;
		else if (str[readPos] == '1')
			op->A.module->inPins[i].type = MP_PULLUP;

		if (str[readPos] == '$' || str[readPos] == 'W')
			sscanf(str + readPos, "%*[$W]%d%*[,)] %n", &op->A.module->inPins[i].pinId, &read);
		else
			sscanf(str + readPos, "%*d%*[,)] %n", &read);
		readPos += read;
	}
	return op;
}

void parser_free_operation(Operation *operation) {
	if (operation->type == OP_CONST || operation->type == OP_PIN || operation->type == OP_WIRE) {
		free(operation);
		return;
	} else if (operation->type == OP_NOT) {
		parser_free_operation(operation->A.op);
		free(operation);
		return;
	} else if (operation->type == OP_MODULE) {
		parser_free_function(&operation->A.module->function);
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
	free(assign->pins);
}

void parser_free_function(FunctionData *functionData) {
	for (int i = 0; i < functionData->assignC; i++)
		parser_free_assign(&functionData->assigns[i]);
	free(functionData->assigns);
	free(functionData->wires);
}