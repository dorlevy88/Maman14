//
// Created by Or Zamir on 7/17/16.
//
#ifndef MAMAN14_SYMBOLSTABLE_H
#define MAMAN14_SYMBOLSTABLE_H

#include <stdbool.h>

typedef struct SymbolRecord {
    char* label;
    int address;
    bool isExternal;
    bool isCommand;
} SymbolRecord;

typedef struct SymbolsTable {
    SymbolRecord* records;
    int recordSize;
    int size;
} SymbolsTable;


int isLabelExistsInTable(SymbolsTable table, char* label);

SymbolRecord getSymbolRecord(SymbolsTable table, char* label);

bool AddNewLabelToTable(SymbolsTable table, char *label, int address, bool isExternal, bool isCommand);

bool SetLabelAddressInTable(SymbolsTable table, char* label, int address);

#endif //MAMAN14_SYMBOLSTABLE_H