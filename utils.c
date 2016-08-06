#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <memory.h>

#include "utils.h"

/*
 *
void print_output(const char* file, int line, char* msg, ...);

 */

void printSyntaxError(const char *msg, const char *filename, int lineNum) {
    fprintf(stderr, "Syntax Error: %s ---> file: %s, line %d\n", msg, filename, lineNum);
}

void PrintCompileError(const char* msg, const char* param, const char* filename, int lineNum){
    fprintf(stderr, "Compiler Error: %s %s ---> file: %s, line %d\n", msg, param, filename, lineNum);
}

void PrintCompileWarning(const char* msg, const char* param, const char* filename, int lineNum){
    fprintf(stderr, "Compiler Warning: %s %s ---> file: %s, line %d\n", msg, param, filename, lineNum);
}

void PrintProcessStep(const char* msg, const char* param, const char* filename){
    fprintf(stderr, "Step: %s %s ---> file: %s", msg, param, filename);
}

void printInternalError(const char *msg, const char *param){
    fprintf(stderr, "Internal Error: %s %s\n", msg, param);
}


char* getNewString(int size) {
    char* res;

    size_t sizeT = ((size_t)size+1) * sizeof(char);
    res = (char*)malloc(sizeT);
    if (res == NULL) {
        printInternalError(ERR_PROG_MEMORY_FAILURE, " Trying to allocate new string");
        exit(0);
    }
    memset(res, 0, sizeT);
    res[size] = '\0';
    return res;
}

char* getNewSubString(const char* pos, int size) {
    char* res;

    res = getNewString(size);
    strncpy(res, pos, (size_t)size);
    return res;
}

char* copyString(const char* pos) {
    return getNewSubString(pos, strlen(pos));
}


int ConvertCompliment2(int num, int size) {
    if (num < 0){
        return (int)(pow(2, size) + num);
    }
    return num;
}
