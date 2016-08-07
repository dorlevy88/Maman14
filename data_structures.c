#include <memory.h>
#include <stdlib.h>
#include "utils.h"
#include <stdio.h>
#include <stdbool.h>

#include "data_structures.h"

#define DATA_BYTE_SIZE 15

/* **********************************************************************
 *
 *     HANDLE ASSEMBLY BYTE STRUCTURES
 *
 * **********************************************************************/

bool pushByteFromInt(AssemblyBytes *bytes, int byte) {
    if(bytes->size == MAX_ASSEMBLY_BYTES) {
        return false;
    }
    bytes->array[bytes->size++] = byte;
    return true;
}

bool pushBytesFromIntArray(AssemblyBytes *bytes, int *array, int arraySize) {
    int i;
    for (i = 0; i < arraySize; ++i) {
        if(pushByteFromInt(bytes, convertCompliment2(array[i], DATA_BYTE_SIZE)) == false) {
            return false;
        }
    }
    return true;
}

bool pushBytesFromString(AssemblyBytes *bytes, char *string) {
    int i;
    for (i = 0; i < strlen(string); ++i) {
        if(pushByteFromInt(bytes, (int) string[i]) == false) {
            return false;
        }
    }
    if (pushByteFromInt(bytes, (int) '\0') == false) {
        return false;
    }
    return true;
}

void printAssemblyByte(AssemblyBytes* bytes){
    int i;
    printf("----------------------------------------------------------------------------------------------------\n");
    for (i = 0; i < bytes->size; i++) {
        printf("%d\t%015ld\n", i, decimalToBinary(bytes->array[i]));
    }
    printf("----------------------------------------------------------------------------------------------------\n");
}

/* **********************************************************************
 *
 *     HANDLE ASSEMBLY STRUCTURE MEMORY
 *
 * **********************************************************************/

bool initAssemblyStructure(AssemblyStructure** assembly) {
    AssemblyBytes* codeBytes;
    AssemblyBytes* dataBytes;
    SymbolsTable* table;
    SymbolRecord* record;
    SymbolsTable* tableExt;
    SymbolRecord* recordExt;

    (*assembly) = (AssemblyStructure*)malloc(sizeof(AssemblyStructure));
    if ((*assembly) == NULL)
        return false;
    memset((*assembly), 0, sizeof(AssemblyStructure));
    (*assembly)->startAddress = ASSEMBLY_CODE_START_ADDRESS;

    codeBytes = (AssemblyBytes*)malloc(sizeof(AssemblyBytes));
    if (codeBytes == NULL)
        return false;
    memset(codeBytes, 0, sizeof(AssemblyBytes));
    (*assembly)->codeArray = codeBytes;

    dataBytes = (AssemblyBytes*)malloc(sizeof(AssemblyBytes));
    if (dataBytes == NULL)
        return false;
    memset(dataBytes, 0, sizeof(AssemblyBytes));
    (*assembly)->dataArray = dataBytes;


    table = (SymbolsTable*)malloc(sizeof(SymbolsTable));
    if (table == NULL)
        return false;
    memset(table, 0, sizeof(SymbolsTable));

    record = (SymbolRecord*)malloc(sizeof(SymbolRecord));
    if (record == NULL)
        return false;
    memset(record, 0, sizeof(SymbolRecord));

    table->records = record;
    table->size = 1;
    (*assembly)->symbolsTable = table;

    tableExt = (SymbolsTable*)malloc(sizeof(SymbolsTable));
    if (tableExt == NULL)
        return false;
    memset(tableExt, 0, sizeof(SymbolsTable));

    recordExt = (SymbolRecord*)malloc(sizeof(SymbolRecord));
    if (recordExt == NULL)
        return false;
    memset(recordExt, 0, sizeof(SymbolRecord));

    tableExt->records = recordExt;
    tableExt->size = 1;
    (*assembly)->externs = tableExt;

    return true;
}

void freeAssemblyStructure(AssemblyStructure** assembly){
    int i;
    if ((*assembly)->codeArray != NULL) {
        printf("Clears Code Array\n");
        free((*assembly)->codeArray);
    }
    if ((*assembly)->dataArray != NULL){
        printf("Clears Data Array\n");
        free((*assembly)->dataArray);
    }

    if ((*assembly)->symbolsTable != NULL){
        printf("Clears Symbols\n");
        printSymbolTable((*assembly)->symbolsTable);
        for (i = 0; i < (*assembly)->symbolsTable->size; ++i) {
            if ((*assembly)->symbolsTable->records[i].label != NULL)
                printf("Clear Record: %s\n", (*assembly)->symbolsTable->records[i].label);
                free((*assembly)->symbolsTable->records[i].label);
        }
        free((*assembly)->symbolsTable->records);
        free((*assembly)->symbolsTable);
        printf("Cleared Symbols\n");
    }

    if ((*assembly)->externs != NULL){
        printf("Clears Externs\n");
        printSymbolTable((*assembly)->externs);
        for (i = 0; i < (*assembly)->externs->size; ++i) {
            if ((*assembly)->externs->records[i].label != NULL)
                free((*assembly)->externs->records[i].label);
        }
        free((*assembly)->externs->records);
        free((*assembly)->externs);
        printf("Cleared Externs\n");
    }

    free(*assembly);
}

/* **********************************************************************
 *
 *     HANDLE SYMBOLS TABLE STRUCTURE
 *
 * **********************************************************************/

int isLabelExistsInTable(SymbolsTable* table, char* label) {
    int i;
    for (i = 0; i < table->recordSize; ++i) {
        if (strcmp(label, table->records[i].label) == 0) {
            return i;
        }
    }
    return LABEL_NOT_EXISTS;
}

bool addNewLabelToTable(SymbolsTable *table, char *label, int address, bool isExternal, bool isCommand, bool isEntry,
                        int byteCodeForDynamic) {
    int labelPos;
    labelPos = isLabelExistsInTable(table, label);
    if(labelPos != LABEL_NOT_EXISTS) {
        return false;
    }
    if (table->recordSize == table->size) { /* Allocate additional space to the array */
        table->records = (SymbolRecord *) realloc(table->records, (table->size + 1) * sizeof(SymbolRecord));
        if (table->records == NULL) {
            exit(0);
        }
        memset(&table->records[table->size], 0, sizeof(SymbolRecord));
        table->size++;
    }
    table->records[table->recordSize].label = copyString(label);
    table->records[table->recordSize].address = address;
    table->records[table->recordSize].isExternal = isExternal;
    table->records[table->recordSize].isCommand = isCommand;
    table->records[table->recordSize].isEntry = isEntry;
    table->records[table->recordSize].byteCodeForDynamic = byteCodeForDynamic;
    table->recordSize++;
    return true;
}

bool addNewExternToTable(SymbolsTable *table, char *label, int address) {
    if (table->recordSize == table->size) { /* Allocate additional space to the array */
        table->records = (SymbolRecord *) realloc(table->records, (table->size + 1) * sizeof(SymbolRecord));
        if (table->records == NULL) {
            exit(0);
        }
        memset(&table->records[table->size], 0, sizeof(SymbolRecord));
        table->size++;
    }
    table->records[table->recordSize].label = copyString(label);
    table->records[table->recordSize].address = address;
    table->records[table->recordSize].isExternal = false;
    table->records[table->recordSize].isCommand = false;
    table->records[table->recordSize].isEntry = false;
    table->records[table->recordSize].byteCodeForDynamic = false;
    table->recordSize++;
    return true;
}

bool setLabelIsEntryInTable(SymbolsTable *table, char *label){
    int labelPos;
    labelPos = isLabelExistsInTable(table, label);
    if (labelPos == LABEL_NOT_EXISTS) {
        return false;
    }

    table->records[labelPos].isEntry = true;
    return true;
}

void printSymbolTable(SymbolsTable* table){
    int i;
    printf("----------------------------------------------------------------------------------------------------\n");
    printf("label\t\taddress\tisExternal\tisCommand\tisEntry\tbyteCodeForDynamic\n");\
    for (i = 0; i < table->recordSize ; i++) {
        printf("%s\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", table->records[i].label, table->records[i].address,
               table->records[i].isExternal, table->records[i].isCommand, table->records[i].isEntry, table->records[i].byteCodeForDynamic);
    }
    printf("----------------------------------------------------------------------------------------------------\n");
}