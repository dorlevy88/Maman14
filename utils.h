#ifndef MAMAN14_LOGGER_H
#define MAMAN14_LOGGER_H

#include "utils.h"

void PrintSyntaxError(char* errorMsg, int lineNum);

void PrintCompileError(char* errorMsg, char* param, int lineNum);

void PrintCompileWarning(char* errorMsg, char* param, int lineNum);

void PrintProcessStep(char* message, char* param);

int ConvertCompliment2(int num, int size);

char* getNewSubString(const char* pos, int size);

char* copyString(const char* pos);

char* getNewString(int size);

#endif /* MAMAN14_LOGGER_H */
