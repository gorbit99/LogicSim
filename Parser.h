#ifndef HOMEWORK_PARSER_H
#define HOMEWORK_PARSER_H

/**
 * @file Parser.h
 * @brief Structures and functions related to parsing function files
 * @version 0.1
 * @date 2019-11-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "Utility.h"
#include <string.h>

/**
 * @brief Type of an operation
 * 
 */
typedef enum OperationType {
    OP_AND, /**< AND gate */
    OP_OR, /**< OR gate */
    OP_XOR, /**< XOR gate */
    OP_NOT, /**< NOT gate */
    OP_PIN, /**< Pin */
    OP_WIRE, /**< Inner wire */
    OP_CONST, /**< Constant value */
    OP_MODULE, /**< Module */
    OP_LPAREN /**< Left paren (only used in parsing) */
} OperationType;

struct Module;

/**
 * @brief Structure holding data for an operation
 * 
 */
typedef struct Operation {
    OperationType type; /**< Type of the operation */
    /**
     * @brief First operand
     * 
     */
    union A {
        struct Operation *op; /**< First operation */
        int pin; /**< Id of the pin */
        bool c; /**< Value of the constant */
        struct Module *module; /**< Module */
    } A;
    struct Operation *B; /**< Second operation, if exists */
} Operation;

/**
 * @brief Represents the type of a pin in an assignment
 */
typedef enum APType {
	AP_PIN, /**< Pin */
	AP_DONTCARE, /**< Disconnected */
	AP_WIRE /**< Inner wire */
} APType;

/**
 * @brief Struct holding information about a pin in an assignment
 */
typedef struct AssignPin {
	APType type; /**< Type of the pin */
	int pinId; /**< Input pin id, if exists */
} AssignPin;

/**
 * @brief Struct holding assignment data
 * 
 */
typedef struct Assign {
    AssignPin *pins; /**< Data about pins */
    int pinNum; /**< Number of pins */
    Operation *op; /**< Operation */
} Assign;

/**
 * @brief Structure holding function data
 * 
 */
typedef struct FunctionData {
    int inC; /**< Number of inputs */
    int outC; /**< Number of outputs */
    int assignC; /**< Number of assignments */
    Assign *assigns; /**< Array of assignments */
    bool *wires; /**< Inner wire values */
} FunctionData;

/**
 * @brief Type of a module pin
 */
typedef enum MPType {
	MP_PIN, /**< Pin */
	MP_WIRE, /**< Wire */
	MP_PULLUP, /**< Pulled to 1 */
	MP_PULLDOWN /**< Pulled to 0 */
} MPType;

/**
 * @brief Struct holding information about a module pin
 */
typedef struct ModulePin {
	MPType type; /**< Type of the pin */
	int pinId; /**< Id of the pin, if exists */
} ModulePin;

/**
 * @brief Structure holding data for a module
 * 
 */
typedef struct Module {
    FunctionData function;
    ModulePin *inPins;
} Module;

/**
 * @brief Load a function from file
 * 
 * @param path File path
 * @return FunctionData Resulting function
 */
FunctionData parser_load_function(char *path);

/**
 * @brief Handle a single operation
 * 
 * @param op Operation to handle
 * @param in Input values
 * @param wires State of the inner wires
 * @param out Output values
 * @param pins The pins used in the assignment
 */
void parser_handle_operation(Operation *op, const bool *in, bool *wires, bool *out, AssignPin *pins);

/**
 * @brief Run a single assignment
 * 
 * @param assign Assignment to run
 * @param  in Input values
 * @param wires State of the inner wires
 * @param out Output values
 */
void parser_run_assign(Assign *assign, bool *in, bool *wires, bool *out);

/**
 * @brief Run function
 * 
 * @param function Function to run
 * @param in Input values
 * @param out Output values
 */
void parser_run_function(FunctionData *function, bool *in, bool *out);

/**
 * @brief Convert C string to operation
 * 
 * @param str String to convert
 * @return Operation* Resulting operation
 */
Operation *parser_string_to_op(char *str);

/**
 * @brief Convert C string to module
 * 
 * @param str String to convert
 * @return Operation* Resulting operation
 */
Operation *parser_string_to_module(char *str);

/**
 * @brief Free operation
 * 
 * @param operation Operation to free
 */
void parser_free_operation(Operation *operation);

/**
 * @brief Free assignment
 * 
 * @param assign Assignment to free
 */
void parser_free_assign(Assign *assign);

/**
 * @brief Free function
 * 
 * @param functionData Function to free
 */
void parser_free_function(FunctionData *functionData);

#endif //HOMEWORK_PARSER_H