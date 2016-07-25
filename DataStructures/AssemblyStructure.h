//
// Created by Or Zamir on 7/17/16.
//

#ifndef MAMAN14_ASSEMBLYSTRUCTURE_H
#define MAMAN14_ASSEMBLYSTRUCTURE_H

#endif //MAMAN14_ASSEMBLYSTRUCTURE_H


typedef struct AssemblyStructure {

    int ic;
    int dc;

    struct AssemblyByte** codeArray;
    struct AssemblyByte** dataArray;

    struct SymbolsTable* symbolsTable;
    struct SymbolsTable* externSymbolsTable;
} AssemblyStructure;

AssemblyStructure* InitAssemblyStructure();