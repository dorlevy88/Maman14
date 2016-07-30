//
// Created by Or Zamir on 7/17/16.
//
#ifndef MAMAN14_ASSEMBLYSTRUCTURE_H
#define MAMAN14_ASSEMBLYSTRUCTURE_H


#include "AssemblyByte.h"
#include "SymbolsTable.h"

typedef struct AssemblyStructure {

    int ic;
    int dc;

    AssemblyByte* codeArray;
    AssemblyByte* dataArray;

    SymbolsTable symbolsTable;
} AssemblyStructure;


//AssemblyStructure InitAssemblyStructure();


#endif //MAMAN14_ASSEMBLYSTRUCTURE_H