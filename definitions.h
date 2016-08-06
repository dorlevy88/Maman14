#ifndef MAMAN14_DEFINITIONS_H
#define MAMAN14_DEFINITIONS_H

/* Defines for General Errors */
#define ERR_COMPILER_MEMORY_FAILURE "Compiler internal memory error"

/* Defines for Syntax Errors */

/* Defines for Compiler Errors */
#define ERR_RAM_OVERFLOW "The program is too big for the computer RAM"
#define ERR_LABEL_DEFINED_TWICE "The label defined twice"
#define ERR_LABEL_NOT_DEFINED "The label definition doesn't exists"
#define WARN_LABEL_IN_BAD_LOCATION "Label is not allowed on .extern or .entry"


/* Defining enums */
typedef enum ActionTypes {
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

#endif /* MAMAN14_DEFINITIONS_H */
