#ifndef MAMAN14_UTILS_H
#define MAMAN14_UTILS_H

#include "definitions.h"
#include "utils.h"


void printSyntaxError(char *msg, const char *filename, int lineNum);

void printCompileError(char *msg, const char *filename, int lineNum);

void printCompileWarning(char *msg, const char *filename, int lineNum);

void printProcessStep(const char *msg, const char *filename);

void printInternalError(const char *msg, const char *param);

int convertCompliment2(int num, int size);

char* getNewSubString(const char* pos, int size);

char* copyString(const char* pos);

char* errMessage(const char *err, const char *param);

char* getNewString(int size);

long decimalToBinary(int n);

#endif /* MAMAN14_UTILS_H */
