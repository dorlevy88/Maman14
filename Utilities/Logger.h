//
// Created by Dor Levy on 7/30/16.
//

#ifndef MAMAN14_LOGGER_H
#define MAMAN14_LOGGER_H

void PrintSyntaxError(char* errorMsg, int lineNum);

void PrintCompileError(char* errorMsg, char* param, int lineNum);

void PrintCompileWarning(char* errorMsg, char* param, int lineNum);

#endif //MAMAN14_LOGGER_H
