//
// Created by Or Zamir on 7/17/16.
//
#ifndef MAMAN14_ASSEMBLYSTRUCTURE_H
#define MAMAN14_ASSEMBLYSTRUCTURE_H


#include "AssemblyBytes.h"
#include "SymbolsTable.h"

typedef struct AssemblyStructure {

    int ic;
    int dc;

    AssemblyBytes codeArray;
    AssemblyBytes dataArray;

    SymbolsTable symbolsTable;
} AssemblyStructure;


#endif //MAMAN14_ASSEMBLYSTRUCTURE_H