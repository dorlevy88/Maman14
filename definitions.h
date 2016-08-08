#ifndef MAMAN14_DEFINITIONS_H
#define MAMAN14_DEFINITIONS_H

#include <stdbool.h>

#define DEBUG false
#define PRINT_STEPS true

/* Defines for General Errors */
#define ERR_PROG_MEMORY_FAILURE "Program internal memory error"
#define ERR_FILE_NOT_FOUND "File does not exists"
#define EER_NEW_FILE_FAILURE "Error trying to create new file"
#define ERR_RAM_OVERFLOW "Assembly code & data is too big for the computer RAM"
#define ERR_CODE_RAM_OVERFLOW "Assembly code is too big for the computer RAM"
#define ERR_DATA_RAM_OVERFLOW "Assembly data is too big for the computer RAM"

/* Defines for Syntax Errors */
#define ERR_INVAILD_LABEL_EMPTY "Empty Label"
#define ERR_INVAILD_LABEL_FIRST "First character of a label must be a letter"
#define ERR_INVAILD_LABEL_MAX "Label size is more than max allowed characters"
#define ERR_INVAILD_LABEL_ILLEGAL "Label contains illegal characters"
#define ERR_ILLEGAL_DEST_ADDRESSING "Illegal Destination Operand Addressing, Allowed addressing types are DIRECT(1), REGISTER(3) for command"
#define ERR_LEA_SOURCE_ADDRESSING "Illegal Source Operand Addressing on lea command, allowed addressing type is DIRECT(1)"
#define ERR_UNKNOWN_CMD "Unknown Command"
#define ERR_TWO_OP_GOT_ONE "Command expected two operands, received one"
#define ERR_TWO_OP_GOT_MORE "Command expected two operands, received more"
#define ERR_ONE_OP_GOT_NONE "Command expected one operand, received none"
#define ERR_ONE_OP_GOT_MORE "Command expected one operand, received more"
#define ERR_NO_OP_GOT_MORE "Command expected no operand, received more"
#define ERR_DATA_OUT_OF_BOUNDS "Data integer is out of bounds"
#define ERR_STRING_INVALID "String parameter is invalid"
#define ERR_UNKNOWN_ADDRESSING "Unknown addressing type"
#define ERR_INVALID_NUMBER "Expected a number, but received"
#define ERR_NUM_OUT_OF_BOUNDS "Integer is invalid or out of bounds"
#define ERR_BAD_DYNAMIC_ADDRESSING "Incorrect dynamic addressing format"

/* Defines for Compiler Errors */
#define ERR_LABEL_DEFINED_TWICE "The label defined twice"
#define ERR_LABEL_NOT_DEFINED "The label definition doesn't exists"
#define WARN_LABEL_IN_BAD_LOCATION "Label is not allowed on .extern or .entry"
#define ERR_LABEL_NOT_FOUND "Label not found, should exist in second transition"
#define ERR_DYNAM_ADDRESS_EXTERN "Dynamic addressing not relevant for external label"

/* Defining enums */
typedef enum ActionTypes {
    UNKNOWN = -1,

    MOV = 0,
    CMP = 1,
    ADD = 2,
    SUB = 3,
    NOT = 4,
    CLR = 5,
    LEA = 6,
    INC = 7,
    DEC = 8,
    JMP = 9,
    BNE = 10,
    RED = 11,
    PRN = 12,
    JSR = 13,
    RTS = 14,
    STOP = 15,

    DATA = 16,
    STRING = 17,
    EXTERN = 18,
    ENTRY = 19

} ActionTypes;

typedef enum OperandAddressingType {
    NUMBER = 0, /* Miyadi */
    DIRECT = 1, /* Yashir */
    DYNAMIC = 2, /* Miyadi Dinami */
    REGISTER = 3 /* Oger Yashir */
} OperandAddressingType;

typedef enum CommandAREType {
    Absolute = 0,       /* A */
    Relocatable = 2,    /* R */
    External = 1        /* E */
} CommandAREType;

typedef enum Status {
    Pass = true,
    Fail = false
} Status;

#endif /* MAMAN14_DEFINITIONS_H */
