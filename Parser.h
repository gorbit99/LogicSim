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
 * @brief Struct holding assignment data
 * 
 */
typedef struct Assign {
    int *outPins; /**< Ids of the output pins */
    int outNum;
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
} FunctionData;

/**
 * @brief Structure holding data for a module
 * 
 */
typedef struct Module {
    FunctionData function;
    int *inPins;
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
 * @param out Output values
 */
void parser_handle_operation(Operation *op, bool *in, bool *out);

/**
 * @brief Run a single assignment
 * 
 * @param assign Assignment to run
 * @param in Input values
 * @param out Output values
 */
void parser_run_assign(Assign *assign, bool *in, bool *out);

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