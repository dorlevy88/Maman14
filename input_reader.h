#ifndef MAMAN14_INPUTREADER_H
#define MAMAN14_INPUTREADER_H

#include "data_structures.h"
#include "utils.h"
#include <stdbool.h>

/*
 * Maman 14 for the following students -
 * 1. Or Zamir - 301222063
 * 2. Dor Levy - 301641098
 */

/* **********************************************************************
 *
 *     FILE HANDLERS DEFINITIONS
 *
 * **********************************************************************/

typedef struct Operand {

    OperandAddressingType addressingType;

    /* in case of command */
    int registerNum;  /* register addressing */
    int value;                  /* number addressing - #3 -> 3 */
    char* label;                /* direct\dynamic addressing - X[1-6] -> X */
    int maxNum;                 /* dynamic addressing max position - X[1-6] -> 6 */
    int minNum;                 /* dynamic addressing min position - X[1-6] -> 1 */

    /* in case of .data\.string */
    int* data; /*  could be multiple int in case of .data */
    int dataSize;
    char* string; /*  only one string in case of .string */

    /* in case .entry\.extern */
    char* entryOrExtern; /*  label of entry/extern */

} Operand;

typedef struct FileLine {
    int lineNumber;
    char* originalLine;
    bool isEmptyOrComment;

    char* label;
    char* action;
    ActionTypes actionType;
    int numOfCommandOprands;

    Operand* firstOperValue;
    Operand* secondOperValue;

} FileLine;

typedef struct FileContent {
    char* filename;
    FileLine* line;
    int size;

} FileContent;

/* **********************************************************************
 *
 *     FILE HANDLERS METHODS
 *
 * **********************************************************************/

Status getFileContent(char *filename, FileContent *fileContent) ;

Status initFileContent(FileContent** fileContent);

void freeFileContent(FileContent** fileContent);

#endif /* MAMAN14_INPUTREADER_H */