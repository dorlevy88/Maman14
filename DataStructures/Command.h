//
// Created by Dor Levy on 7/10/16.
//
#ifndef MAMAN14_COMMAND_H
#define MAMAN14_COMMAND_H

#include "../Utilities/FileHandlers.h"
#include "SymbolsTable.h"

int buildBinaryCommand(FileLine cmdLine);

int buildBinaryData(Operand* operand, SymbolsTable* table);

#endif //MAMAN14_COMMAND_H
