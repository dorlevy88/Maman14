//
// Created by Or Zamir on 7/16/16.
//


#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include "SymbolsTable.h"

#define LABEL_NOT_EXISTS -1
#define NEW_CHUNK_SIZE 10

/// Returns the index of the symbol, if not exists returns -1
/// \param table
/// \param label
/// \return
int isLabelExistsInTable(SymbolsTable table, char* label) {
    for (int i = 0; i < table.recordSize; ++i) {
        if (strcmp(label, table.records[i].label)) {
            return i;
        }
    }
    return LABEL_NOT_EXISTS;
}

SymbolRecord getSymbolRecord(SymbolsTable table, char* label) {
    //TODO:Implement getSymbolRecord
    return NULL;
}


bool AddNewLabelToTable(SymbolsTable table, char *label, int address, bool isExternal, bool isCommand) {
    int labelPos = isLabelExistsInTable(table, label);
    if(labelPos != LABEL_NOT_EXISTS) {
        return false;
    }
    if (table.recordSize == table.size) { //Allocate additional space to the array
        table.records = (SymbolRecord *) malloc((table.size + NEW_CHUNK_SIZE) * sizeof(SymbolRecord));
        table.size += NEW_CHUNK_SIZE;
    }
    table.recordSize++;
    table.records[table.recordSize] = {
            label: label,
            address: address,
            isExternal: isExternal,
            isCommand: isCommand
    };

    return true;
}

bool SetLabelAddressInTable(SymbolsTable table, char* label, int address) {
    int labelPos = isLabelExistsInTable(table, label);
    if (labelPos == LABEL_NOT_EXISTS) {
        return false;
    }

    table.records[labelPos].address = address;
    return true;
}