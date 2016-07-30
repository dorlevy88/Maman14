//
// Created by Or Zamir on 7/16/16.
//


#include <stddef.h>
#include "SymbolsTable.h"

/// Returns the index of the symbol, if not exists returns -1
/// \param table
/// \param label
/// \return
int isLabelExistsInTable(SymbolsTable table, char* label) {
    //TODO:Implement isLabelExistInTable
    return -1;
}

SymbolRecord getSymbolRecord(SymbolsTable table, char* label) {
    //TODO:Implement getSymbolRecord
    return NULL;
}


bool CheckAndAddLabelToTable(SymbolsTable table, char* label, int address, bool isExternal, bool isCommand) {
    int labelPos = isLabelExistsInTable(table, label);
    if(labelPos != -1) {
        return false;
    }
    SymbolRecord rec = {
            label: label,
            address: address,
            isExternal: isExternal,
            isCommand: isCommand
    };
    table.record[++table.size] = rec;
    return true;
}