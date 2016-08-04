//
// Created by Or Zamir on 7/31/16.
//

#include <stdlib.h>
#include "AssemblyStructure.h"

AssemblyStructure* InitAssemblyStructure() {

    AssemblyStructure* assembly = (AssemblyStructure*)malloc(sizeof(AssemblyStructure));
    assembly->startAddress = ASSEMBLY_CODE_START_ADDRESS;

    AssemblyBytes* codeBytes = (AssemblyBytes*)malloc(sizeof(AssemblyBytes));
    assembly->codeArray = codeBytes;
    AssemblyBytes* dataBytes = (AssemblyBytes*)malloc(sizeof(AssemblyBytes));
    assembly->dataArray = dataBytes;

    SymbolsTable* table = (SymbolsTable*)malloc(sizeof(SymbolsTable));
    memset(table, 0, sizeof(SymbolsTable));
    SymbolRecord* record = (SymbolRecord*)malloc(sizeof(SymbolRecord));
    memset(record, 0, sizeof(SymbolRecord));
    table->records = record;
    table->size = 1;
    assembly->symbolsTable = table;

    return assembly;
}