//
// Created by Dor Levy on 7/30/16.
//

#include <stdio.h>
#include "Logger.h"


void PrintSyntaxError(char* errorMsg, int lineNum){
    fprintf(stderr, "Syntax Error in line %d - %s", lineNum, errorMsg);
}

void PrintCompileError(char* errorMsg, int lineNum){
    fprintf(stderr, "Compile Error in line %d - %s", lineNum, errorMsg);
}

void PrintCompileWarning(char* errorMsg, int lineNum){
    fprintf(stderr, "Compile Warning in line %d - %s", lineNum, errorMsg);
}