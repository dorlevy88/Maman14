#ifndef MAMAN14_LOGGER_H
#define MAMAN14_LOGGER_H

#include "utils.h"


/*
void print_error(const char* file, int line, char* msg);

void print_output(const char* file, int line, char* msg, ...);

*/
void PrintSyntaxError(const char* errorMsg, int lineNum);

void PrintCompileError(const char* errorMsg, const char* param, int lineNum);

void PrintCompileWarning(const char* errorMsg, const char* param, int lineNum);

void PrintProcessStep(const char* message, const char* param);


int ConvertCompliment2(int num, int size);

char* getNewSubString(const char* pos, int size);

char* copyString(const char* pos);

char* getNewString(int size);

#endif /* MAMAN14_LOGGER_H */
