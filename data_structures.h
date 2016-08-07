#ifndef MAMAN14_DATASTRUCTURES_H
#define MAMAN14_DATASTRUCTURES_H

#include <stdbool.h>
#include "definitions.h"

#define MAX_ASSEMBLY_BYTES 1000
#define LABEL_NOT_EXISTS -1
#define ASSEMBLY_CODE_START_ADDRESS 100

/* **********************************************************************
 *
 *     DATA STRUCTURES DEFINITIONS
 *
 * **********************************************************************/

typedef struct SymbolRecord {
    char* label;
    int address;
    bool isExternal;
    bool isCommand;
    bool isEntry;
    int byteCodeForDynamic;
} SymbolRecord;

typedef struct SymbolsTable {
    SymbolRecord* records;
    int recordSize;
    int size;
} SymbolsTable;

typedef struct AssemblyBytes {

    int array[MAX_ASSEMBLY_BYTES];
    int size;

} AssemblyBytes;

typedef struct AssemblyStructure {

    int ic;
    int dc;

    AssemblyBytes* codeArray;
    AssemblyBytes* dataArray;

    SymbolsTable* symbolsTable;
    SymbolsTable* externs;

} AssemblyStructure;

/* **********************************************************************
 *
 *     HANDLE ASSEMBLY BYTE STRUCTURES
 *
 * **********************************************************************/

bool pushByteFromInt(AssemblyBytes *bytes, int byte);

bool pushBytesFromIntArray(AssemblyBytes *bytes, int *array, int arraySize);

bool pushBytesFromString(AssemblyBytes *bytes, char *string);

void printAssemblyByte(AssemblyBytes* bytes);

/* **********************************************************************
 *
 *     HANDLE ASSEMBLY STRUCTURE MEMORY
 *
 * **********************************************************************/

Status initAssemblyStructure(AssemblyStructure** assembly);

void freeAssemblyStructure(AssemblyStructure** assembly);

/* **********************************************************************
 *
 *     HANDLE SYMBOLS TABLE STRUCTURE
 *
 * **********************************************************************/

int isLabelExistsInTable(SymbolsTable* table, char* label);

bool addNewLabelToTable(SymbolsTable *table, char *label, int address, bool isExternal, bool isCommand, bool isEntry,
                        int byteCodeForDynamic);

bool addNewExternToTable(SymbolsTable *table, char *label, int address);

bool setLabelIsEntryInTable(SymbolsTable *table, char *label);

void printSymbolTable(SymbolsTable* table);

#endif /* MAMAN14_DATASTRUCTURES_H */
