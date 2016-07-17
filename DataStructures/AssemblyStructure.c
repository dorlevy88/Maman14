//
// Created by Or Zamir on 7/17/16.
//

#include "AssemblyStructure.h"
#include "AssemblyByte.h"
#include "SymbolsTable.h"

struct AssemblyStructure {

    int ic;
    int dc;

    struct AssemblyByte** codeArray;
    struct AssemblyByte** dataArray;

    struct SymbolsTable* symbolsTable;
    struct SymbolsTable* externSymbolsTable;
};

struct AssemblyStructure* InitAssemblyStructure() {
    AssemblyStructure* assemblyStructure = {
            0,0,


    };
}