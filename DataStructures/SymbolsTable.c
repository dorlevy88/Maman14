//
// Created by Or Zamir on 7/16/16.
//


#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <printf.h>
#include "SymbolsTable.h"

#define NEW_CHUNK_SIZE 10

/// Returns the index of the symbol, if not exists returns -1
/// \param table
/// \param label
/// \return
int isLabelExistsInTable(SymbolsTable* table, char* label) {
    for (int i = 0; i < table->recordSize; ++i) {
        if (strcmp(label, table->records[i].label) == 0) {
            return i;
        }
    }
    return LABEL_NOT_EXISTS;
}

bool AddNewLabelToTable(SymbolsTable* table, char *label, int address, bool isExternal, bool isCommand, int byteCodeForDynamic) {
    int labelPos = isLabelExistsInTable(table, label);
    if(labelPos != LABEL_NOT_EXISTS) {
        return false;
    }
    if (table->recordSize == table->size) { //Allocate additional space to the array
        table->records = (SymbolRecord *) realloc(table->records, table->size + (NEW_CHUNK_SIZE * sizeof(SymbolRecord)));
        table->size += NEW_CHUNK_SIZE;
    }
    table->records[table->recordSize].label = label;
    table->records[table->recordSize].address = address;
    table->records[table->recordSize].isExternal = isCommand;
    table->records[table->recordSize].isCommand = isCommand;
    table->records[table->recordSize].byteCodeForDynamic = byteCodeForDynamic;
    table->recordSize++;
    return true;
}

bool SetLabelAddressInTable(SymbolsTable* table, char* label, int address, int byteCodeForDynamic) {
    int labelPos = isLabelExistsInTable(table, label);
    if (labelPos == LABEL_NOT_EXISTS) {
        return false;
    }

    table->records[labelPos].address = address;
    if (byteCodeForDynamic != DYNAMIC_ADDRESSING_NOT_AVAILABLE)
        table->records[labelPos].byteCodeForDynamic = byteCodeForDynamic;

    return true;
}

void printSymbolTable(SymbolsTable* table){
    printf("----------------------------------------------------------------------------------------------------");
    printf("label\taddress\tisExternal\tisCommand\tbyteCodeForDynamic\n");
    for (int i = 0; i < table->recordSize ; i++) {
        printf("%s\t%d\t%d\t%d\t%d\n", table->records[i].label, table->records[i].address,
               table->records[i].isExternal, table->records[i].isCommand, table->records[i].byteCodeForDynamic);
    }
    printf("----------------------------------------------------------------------------------------------------");
}