#include <stdio.h>
#include <math.h>


void PrintSyntaxError(char* errorMsg, int lineNum){
    fprintf(stderr, "Syntax Error in line %d - %s\n", lineNum, errorMsg);
}

void PrintCompileError(char* errorMsg, char* param, int lineNum){
    fprintf(stderr, "Compile Error in line %d - %s, Parameter: %s\n", lineNum, errorMsg, param);
}

void PrintCompileWarning(char* errorMsg, char* param, int lineNum){
    fprintf(stderr, "Compile Warning in line %d - %s, Parameter: %s\n", lineNum, errorMsg, param);
}


void PrintProcessStep(char* message, char* param){
    fprintf(stdout, "Message - %s, Parameter: %s\n", message, param);
}


int ConvertCompliment2(int num, int size) {
    if (num < 0){
        return (int)(pow(2, size) + num);
    }
    return num;
}