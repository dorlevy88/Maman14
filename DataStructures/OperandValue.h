//
// Created by Or Zamir on 7/30/16.
//

#ifndef MAMAN14_OPERANDVALUE_H
#define MAMAN14_OPERANDVALUE_H

#include "Enums.h"

typedef struct OperandValue {

    //in case of command
    RegisterType registerType;  //register addressing
    int value;                  //number addressing
    char* label;                //direct\dynamic addressing
    int maxNum;                 //dynamic addressing max position
    int minNum;                 //dynamic addressing min position

    //in case of .data\.string
    int* data;
    char* string;

    //in case .entry\.extern
    char* entryOrExtern;

} OperandValue;



#endif //MAMAN14_OPERANDVALUE_H
