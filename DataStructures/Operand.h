//
// Created by Or Zamir on 7/30/16.
//

#ifndef MAMAN14_OPERAND_H
#define MAMAN14_OPERAND_H

#include "Enums.h"

typedef struct Operand {

    OperandAddressingType addressingType;

    //in case of command
    int registerNum;  //register addressing
    int value;                  //number addressing - #3 -> 3
    char* label;                //direct\dynamic addressing - X[1-6] -> X
    int maxNum;                 //dynamic addressing max position - X[1-6] -> 6
    int minNum;                 //dynamic addressing min position - X[1-6] -> 1

    //in case of .data\.string
    int* data; // could be multiple int in case of .data
    int dataSize;
    char* string; // only one string in case of .string

    //in case .entry\.extern
    char* entryOrExtern; // label of entry/extern

    //error handling
    bool hasError;
    char* err;

} Operand;



#endif //MAMAN14_OPERAND_H
