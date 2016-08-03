//
// Created by Dor Levy on 7/27/16.
//

#ifndef MAMAN14_ENUMS_H
#define MAMAN14_ENUMS_H


typedef enum ActionTypes {
    UNKNOWN_ACTION = -1,
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
    UNKNOWN_OPERAND_ADDRESSING_TYPE = -1,
    NUMBER = 0, //Miyadi
    DIRECT = 1, //Yashir
    DYNAMIC = 2, //Miyadi Dinami
    REGISTER = 3 //Oger Yashir
} OperandAddressingType;

typedef enum CommandAREType {
    UNKNOWN_COMMAND_ARE_TYPE = -1,
    Absolute = 0,       //A
    Relocatable = 2,    //R
    External = 1        //E
} CommandAREType;


#endif //MAMAN14_ENUMS_H
