#include <stdio.h>
#include "utils.h"


void PrintSyntaxError(char* errorMsg, int lineNum){
    fprintf(stderr, "Syntax Error in line %d - %s", lineNum, errorMsg);
}

void PrintCompileError(char* errorMsg, char* param, int lineNum){
    fprintf(stderr, "Compile Error in line %d - %s, Parameter: %s", lineNum, errorMsg, param);
}

void PrintCompileWarning(char* errorMsg, char* param, int lineNum){
    fprintf(stderr, "Compile Warning in line %d - %s, Parameter: %s", lineNum, errorMsg, param);
}