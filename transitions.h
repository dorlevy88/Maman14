#ifndef MAMAN14_TRANSITIONS_H
#define MAMAN14_TRANSITIONS_H

#include "input_reader.h"
#include "utils.h"

#define OPERAND_BYTE_SIZE 13
#define MAX_CPU_MEMORY 1000

int buildBinaryCommand(FileLine cmdLine);

int buildBinaryData(Operand* operand, SymbolsTable* table, SymbolsTable* externs, bool isDestinationOperand, int cmdAddress);

bool RunFirstTransition(FileContent* fileContent, AssemblyStructure* assembly);

bool RunSecondTransition(FileContent* fileContent, AssemblyStructure* assembly);

#endif /* MAMAN14_TRANSITIONS_H */
