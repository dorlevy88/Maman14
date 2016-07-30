//
// Created by Dor Levy on 7/27/16.
//

#ifndef MAMAN14_ENUMS_H
#define MAMAN14_ENUMS_H


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

typedef enum LineType {
    Data = 0,
    Code = 1
} LineType;

typedef enum OperandAddressingType {
    NUMBER = 0, //Miyadi
    DIRECT = 1, //Yashir
    DYNAMIC = 2, //Miyadi Dinami
    REGISTER = 3 //Oger Yashir
} OperandAddressingType;

typedef enum RegisterType {
    r0,
    r1,
    r2,
    r3,
    r4,
    r5,
    r6,
    r7
} RegisterType;

#endif //MAMAN14_ENUMS_H
