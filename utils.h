#ifndef MAMAN14_LOGGER_H
#define MAMAN14_LOGGER_H

#include "definitions.h"
#include "utils.h"

#include <mem.h>
/*
void print_error(const char* file, int line, char* msg);

void print_output(const char* file, int line, char* msg, ...);
*/

void printSyntaxError(const char *msg, const char *filename, int lineNum);

void PrintCompileError(const char* msg, const char* param, const char* filename, int lineNum);

void PrintCompileWarning(const char* msg, const char* param, const char* filename, int lineNum);

void PrintProcessStep(const char* msg, const char* param, const char* filename);

void printInternalError(const char *msg, const char *param);

int ConvertCompliment2(int num, int size);

char* getNewSubString(const char* pos, int size);

char* copyString(const char* pos);

char* getNewString(int size);

#endif /* MAMAN14_LOGGER_H */
