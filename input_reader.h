#ifndef MAMAN14_FILEHANDLERS_H
#define MAMAN14_FILEHANDLERS_H

#include <stdbool.h>
#include "data_structures.h"
#include "utils.h"

#define MAX_FILE_LINES 1000
#define MAX_LINE_SIZE 80
#define MAX_LABEL_SIZE 30
#define MIN_NUM_SIZE -16384
#define MAX_NUM_SIZE 16384
#define INVALID_NUM_TOKEN -999999
#define MAX_DYNAMIC_OPERAND 13 //max exact number


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

    FileLine line[MAX_FILE_LINES];
    int size;

} FileContent;

bool getFileContent(char* filename, FileContent* fileContent) ;


#endif //MAMAN14_FILEHANDLERS_H