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

bool AddNewLabelToTable(SymbolsTable* table, char *label, int address, bool isExternal, bool isCommand, bool isEntry, int byteCodeForDynamic) {
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
    table->records[table->recordSize].isExternal = isExternal;
    table->records[table->recordSize].isCommand = isCommand;
    table->records[table->recordSize].isEntry = isEntry;
    table->records[table->recordSize].byteCodeForDynamic = byteCodeForDynamic;
    table->recordSize++;
    return true;
}

bool SetLabelIsEntryInTable(SymbolsTable* table, char* label, bool isEntry){
    int labelPos = isLabelExistsInTable(table, label);
    if (labelPos == LABEL_NOT_EXISTS) {
        return false;
    }

    table->records[labelPos].isEntry = isEntry;
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

bool AddExternUsageAddress(SymbolRecord* record, int usedAddress){
//    int labelPos = isLabelExistsInTable(table, externLabel);
//    if (labelPos == LABEL_NOT_EXISTS) {
//        return false;
//    }
    //SymbolRecord* record = &table->records[labelPos];
    record->externUsageAddresses = (int *) realloc(record->externUsageAddresses, record->externUsages + sizeof(int));
    record->externUsageAddresses[record->externUsages] = usedAddress;
    record->externUsages++;

    return true;
}


void printSymbolTable(SymbolsTable* table){
    printf("----------------------------------------------------------------------------------------------------\n");
    printf("label\taddress\tisExternal\tisCommand\tisEntry\tbyteCodeForDynamic\n");
    for (int i = 0; i < table->recordSize ; i++) {
        printf("%s\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", table->records[i].label, table->records[i].address,
               table->records[i].isExternal, table->records[i].isCommand, table->records[i].isEntry, table->records[i].byteCodeForDynamic);
    }
    printf("----------------------------------------------------------------------------------------------------\n");
}