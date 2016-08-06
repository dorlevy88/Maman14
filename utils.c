#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <memory.h>


void PrintSyntaxError(const char* errorMsg, int lineNum){
    fprintf(stderr, "Syntax Error in line %d - %s\n", lineNum, errorMsg);
}

void PrintCompileError(const char* errorMsg, const char* param, int lineNum){
    fprintf(stderr, "Compile Error in line %d - %s, Parameter: %s\n", lineNum, errorMsg, param);
}

void PrintCompileWarning(const char* errorMsg, const char* param, int lineNum){
    fprintf(stderr, "Compile Warning in line %d - %s, Parameter: %s\n", lineNum, errorMsg, param);
}


void PrintProcessStep(const char* message, const char* param){
    fprintf(stdout, "Message - %s, Parameter: %s\n", message, param);
}

int ConvertCompliment2(int num, int size) {
    if (num < 0){
        return (int)(pow(2, size) + num);
    }
    return num;
}


char* getNewString(int size) {
    char* res;

    size_t sizeT = ((size_t)size+1) * sizeof(char);
    res = (char*)malloc(sizeT);
    memset(res, 0, sizeT);
    res[size] = '\0';
    return res;
}

char* getNewSubString(const char* pos, int size) {
    char* res;

    res = getNewString(size);
    strncpy(res, pos, size);
    return res;
}

char* copyString(const char* pos) {
    return getNewSubString(pos, strlen(pos));
}