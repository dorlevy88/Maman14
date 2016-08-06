#ifndef MAMAN14_DATASTRUCTURES_H
#define MAMAN14_DATASTRUCTURES_H

#include <stdbool.h>
#include "definitions.h"

#define MAX_ASSEMBLY_BYTES 1000
#define LABEL_NOT_EXISTS -1
#define ASSEMBLY_CODE_START_ADDRESS 100

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

    int startAddress;
    int ic;
    int dc;

    AssemblyBytes* codeArray;
    AssemblyBytes* dataArray;

    SymbolsTable* symbolsTable;
    SymbolsTable* externs;

} AssemblyStructure;

typedef struct Operand {

    OperandAddressingType addressingType;

    /* in case of command */
    int registerNum;  /* register addressing */
    int value;                  /* number addressing - #3 -> 3 */
    char* label;                /* direct\dynamic addressing - X[1-6] -> X */
    int maxNum;                 /* dynamic addressing max position - X[1-6] -> 6 */
    int minNum;                 /* dynamic addressing min position - X[1-6] -> 1 */

    /* in case of .data\.string */
    int* data; /*  could be multiple int in case of .data */
    int dataSize;
    char* string; /*  only one string in case of .string */

    /* in case .entry\.extern */
    char* entryOrExtern; /*  label of entry/extern */

} Operand;


/* Assembly Bytes Functions */
bool PushByteFromInt(AssemblyBytes* bytes, int byte);

bool PushBytesFromIntArray(AssemblyBytes* bytes, int* array, int arraySize);

bool PushBytesFromString(AssemblyBytes* bytes, char* string);

void printAssemblyByte(AssemblyBytes* bytes);

/* Assembly Structure Init */
bool initAssemblyStructure(AssemblyStructure** assembly);

void freeAssemblyStructure(AssemblyStructure** assembly);

/*  Symbol Table Functions */
int isLabelExistsInTable(SymbolsTable* table, char* label);

bool AddNewLabelToTable(SymbolsTable* table, char *label, int address, bool isExternal, bool isCommand, bool isEntry, int byteCodeForDynamic);

bool SetLabelIsEntryInTable(SymbolsTable* table, char* label, bool isEntry);

void printSymbolTable(SymbolsTable* table);

#endif /* MAMAN14_DATASTRUCTURES_H */
