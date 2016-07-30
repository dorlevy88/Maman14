//
// Created by Dor Levy on 7/26/16.
//

#ifndef MAMAN14_FILEHANDLERS_H
#define MAMAN14_FILEHANDLERS_H

#include <stdbool.h>
#include "../DataStructures/Enums.h"
#include "../DataStructures/OperandValue.h"


#define NUM_OF_LINE_ELEMENTS 4
#define MAX_FILE_LINES 1000
#define MAX_LINE_SIZE 100


typedef struct FileLine {

    char* originalLine;
    int lineNum;
    LineType lineType;
    bool hasSyntaxError;
    bool isEmptyOrComment;

    char* label;
    char* action;
    ActionTypes actionType;

    char* firstOperand;
    OperandAddressingType firstOperType;
    OperandValue firstOperValue;

    char* secondOperand;
    OperandAddressingType secondOperType;
    OperandValue secondOperValue;

} FileLine;


typedef struct FileContent {

    FileLine line[MAX_FILE_LINES];
    int size;
    bool hasError;

} FileContent;

FileContent getFileContent(char* filename);


#endif //MAMAN14_FILEHANDLERS_H