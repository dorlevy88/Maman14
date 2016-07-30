//
// Created by Or Zamir on 7/17/16.
//

#include <stdbool.h>

#ifndef MAMAN14_SYMBOLSTABLE_H
#define MAMAN14_SYMBOLSTABLE_H

typedef struct SymbolRecord {
    char* label;
    int address;
    bool isExternal;
    bool isCommand;
} SymbolRecord;

typedef struct SymbolsTable {
    SymbolRecord* record;
    int size;
} SymbolsTable;


int isLabelExistsInTable(SymbolsTable table, char* label);

SymbolRecord getSymbolRecord(SymbolsTable table, char* label);

#endif //MAMAN14_SYMBOLSTABLE_H