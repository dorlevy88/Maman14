//
// Created by Or Zamir on 7/31/16.
//

#include <stdlib.h>
#include "AssemblyStructure.h"

AssemblyStructure* InitAssemblyStructure() {

    AssemblyStructure* assembly = (AssemblyStructure*)malloc(sizeof(AssemblyStructure));

    AssemblyBytes* codeBytes = (AssemblyBytes*)malloc(sizeof(AssemblyBytes));
    assembly->codeArray = codeBytes;
    AssemblyBytes* dataBytes = (AssemblyBytes*)malloc(sizeof(AssemblyBytes));
    assembly->dataArray = dataBytes;

    SymbolsTable* table = (SymbolsTable*)malloc(sizeof(SymbolsTable));
    SymbolRecord* record = (SymbolRecord*)malloc(sizeof(SymbolRecord)*10);
    table->records = record;
    assembly->symbolsTable = table;

    return assembly;
}