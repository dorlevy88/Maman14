#ifndef MAMAN14_FILEHANDLERS_H
#define MAMAN14_FILEHANDLERS_H

#include "data_structures.h"
#include "utils.h"
#include <stdbool.h>

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

    FileLine* line;
    int size;

} FileContent;

Status getFileContent(char *filename, FileContent *fileContent) ;

Status initFileContent(FileContent** fileContent);
void freeFileContent(FileContent** fileContent);

#endif /* MAMAN14_FILEHANDLERS_H */