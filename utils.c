#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <memory.h>

#include "utils.h"

/*
 * Maman 14 for the following students -
 * 1. Or Zamir - 301222063
 * 2. Dor Levy - 301641098
 */

/**
 * Print syntax error to std error
 * @param msg - message to write
 * @param filename - file name message arrived from
 * @param lineNum - line number where the error occured
 */
void printSyntaxError(char *msg, const char *filename, int lineNum) {
    fprintf(stderr, "Syntax Error: %s ---> file: %s, line %d\n", msg, filename, lineNum);
    free(msg);
}

/**
 * Print compile error to std error
 * @param msg - message to write
 * @param filename - file name message arrived from
 * @param lineNum - line number where the error occured
 */
void printCompileError(char *msg, const char *filename, int lineNum){
    fprintf(stderr, "Compiler Error: %s ---> file: %s, line %d\n", msg, filename, lineNum);
    free(msg);
}

/**
 * Print compile warning to std error
 * @param msg - message to write
 * @param filename - file name message arrived from
 * @param lineNum - line number where the warning occured
 */
void printCompileWarning(char *msg, const char *filename, int lineNum){
    fprintf(stderr, "Compiler Warning: %s ---> file: %s, line %d\n", msg, filename, lineNum);
    free(msg);
}

/**
 * print info messages for each step
 * @param msg - message to write
 * @param filename - file name processing
 */
void printProcessStep(const char *msg, const char *filename){
    if (PRINT_STEPS)
        printf("Step: %s ---> file: %s\n", msg, filename);
}

/**
 * Print internal error to std error
 * @param msg - message to write
 * @param param - string to add to message
 */
void printInternalError(const char *msg, const char *param){
    fprintf(stderr, "Internal Error: %s %s\n", msg, param);
}

/**
 * allocate a new string of size - size
 * @param size - size of string to allocate memory for
 * @return new allocated string
 */
char* getNewString(int size) {
    char* res;

    size_t sizeT = ((size_t)size+1) * sizeof(char);
    res = (char*)malloc(sizeT);
    if (res == NULL) { /* if there was an error allocating memmory, throw error */
        printInternalError(ERR_PROG_MEMORY_FAILURE, " Trying to allocate new string");
        exit(0);
    }
    memset(res, 0, sizeT);
    res[size] = '\0';
    return res;
}

/**
 * gets a string and number of chars to copy from it and returns the copied string
 * @param pos
 * @param size
 * @return
 */
char* getNewSubString(const char* pos, int size) {
    char* res;

    res = getNewString(size); /* allocate new string */
    strncpy(res, pos, (size_t)size); /* copy size chars from original string */
    return res;
}

/**
 * returns a copy of a string
 * @param pos - original string
 * @return copy of string
 */
char* copyString(const char* pos) {
    return getNewSubString(pos, (int)strlen(pos));
}

/**
 * creates a nice error message from error and error parameter
 * @param err
 * @param param
 * @return nice error string
 */
char* errMessage(const char *err, const char *param) {
    char* errMsg = getNewString((int)strlen(err) + 1 + (int)strlen(param));
    strcpy(errMsg, err);
    strcat(errMsg, " ");
    strcat(errMsg, param);
    return errMsg;
}

/**
 * convert signed int to unsigned int using complement's 2
 * @param num - int to convert
 * @param size - number of bits to convert the num to
 * @return unsigned int
 */
int convertCompliment2(int num, int size) {
    if (num < 0){
        return (int)(pow(2, size) + num);
    }
    return num;
}

/**
 * convert base 10 number to base 2 number
 * @param n - number to convert
 * @return number in base 2
 */
long decimalToBinary(int n) {
    int remainder;
    long binary = 0, i = 1;

    while(n != 0) {
        remainder = n%2;
        n = n/2;
        binary= binary + (remainder*i);
        i = i*10;
    }
    return binary;
}