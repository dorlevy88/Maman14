//
// Created by Or Zamir on 7/17/16.
//
#ifndef MAMAN14_SYMBOLSTABLE_H
#define MAMAN14_SYMBOLSTABLE_H

#include <stdbool.h>

#define LABEL_NOT_EXISTS -1
#define DYNAMIC_ADDRESSING_NOT_AVAILABLE -999999

typedef struct SymbolRecord {
    char* label;
    int address;
    bool isExternal;
    bool isCommand;
    int byteCodeForDynamic;
} SymbolRecord;

typedef struct SymbolsTable {
    SymbolRecord* records;
    int recordSize;
    int size;
} SymbolsTable;


int isLabelExistsInTable(SymbolsTable table, char* label);

bool AddNewLabelToTable(SymbolsTable table, char *label, int address, bool isExternal, bool isCommand, int byteCodeForDynamic);

bool SetLabelAddressInTable(SymbolsTable table, char* label, int address, int byteCodeForDynamic);

#endif //MAMAN14_SYMBOLSTABLE_H