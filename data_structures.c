#include <memory.h>
#include <stdlib.h>
#include "data_structures.h"
#include "utils.h"
#include <stdio.h>


#define DATA_BYTE_SIZE 15
#define NEW_CHUNK_SIZE 10

bool PushByteFromInt(AssemblyBytes* bytes, int byte) {
    if(bytes->size == MAX_ASSEMBLY_BYTES) {
        return false;
    }
    bytes->array[bytes->size++] = byte;
    return true;
}

bool PushBytesFromIntArray(AssemblyBytes* bytes, int* array, int arraySize) {
    int i;
    for (i = 0; i < arraySize; ++i) {
        if(PushByteFromInt(bytes, ConvertCompliment2(array[i], DATA_BYTE_SIZE)) == false) {
            return false;
        }
    }
    return true;
}

bool PushBytesFromString(AssemblyBytes* bytes, char* string) {
    int i;
    for (i = 0; i < strlen(string); ++i) {
        if(PushByteFromInt(bytes, (int)string[i]) == false) {
            return false;
        }
    }
    if (PushByteFromInt(bytes, (int)'\0') == false) {
        return false;
    }
    return true;
}

long decimalToBinary(int n) {
    int remainder;
    long binary = 0, i = 1;

    while(n != 0) {
        remainder = n%2;
        n = n/2;
        binary= binary + (remainder*i);
        i = i*10;
    }
    return binary;
}

void printAssemblyByte(AssemblyBytes* bytes){
    int i;
    printf("----------------------------------------------------------------------------------------------------\n");
    for (i = 0; i < bytes->size; i++) {
        printf("%d\t%015ld\n", i, decimalToBinary(bytes->array[i]));
    }
    printf("----------------------------------------------------------------------------------------------------\n");
}

AssemblyStructure* InitAssemblyStructure() {
    AssemblyBytes* codeBytes;
    AssemblyBytes* dataBytes;
    SymbolsTable* table;
    SymbolRecord* record;
    int* externUsages;

    AssemblyStructure* assembly = (AssemblyStructure*)malloc(sizeof(AssemblyStructure));
    assembly->startAddress = ASSEMBLY_CODE_START_ADDRESS;

    codeBytes = (AssemblyBytes*)malloc(sizeof(AssemblyBytes));
    assembly->codeArray = codeBytes;
    dataBytes = (AssemblyBytes*)malloc(sizeof(AssemblyBytes));
    assembly->dataArray = dataBytes;

    table = (SymbolsTable*)malloc(sizeof(SymbolsTable));
    memset(table, 0, sizeof(SymbolsTable));
    record = (SymbolRecord*)malloc(sizeof(SymbolRecord));
    memset(record, 0, sizeof(SymbolRecord));
    externUsages = (int*)malloc(0);
    memset(externUsages, 0, 0);
    record->externUsageAddresses = externUsages;
    table->records = record;
    table->size = 1;
    assembly->symbolsTable = table;

    return assembly;
}

void FreeAssemlyStructureMemory(AssemblyStructure* assembly){
    if (assembly != NULL){
        free(assembly->codeArray);
        free(assembly->dataArray);
        free(assembly->symbolsTable);
        free(assembly);
    }
}

int isLabelExistsInTable(SymbolsTable* table, char* label) {
    int i;
    for (i = 0; i < table->recordSize; ++i) {
        if (strcmp(label, table->records[i].label) == 0) {
            return i;
        }
    }
    return LABEL_NOT_EXISTS;
}

bool AddNewLabelToTable(SymbolsTable* table, char *label, int address, bool isExternal, bool isCommand, bool isEntry, int byteCodeForDynamic) {
    int labelPos;
    labelPos = isLabelExistsInTable(table, label);
    if(labelPos != LABEL_NOT_EXISTS) {
        return false;
    }
    if (table->recordSize == table->size) { /* Allocate additional space to the array */
        table->records = (SymbolRecord *) realloc(table->records, table->size + (NEW_CHUNK_SIZE * sizeof(SymbolRecord)));
        table->size += NEW_CHUNK_SIZE;
    }
    table->records[table->recordSize].label = label;
    table->records[table->recordSize].address = address;
    table->records[table->recordSize].isExternal = isExternal;
    table->records[table->recordSize].isCommand = isCommand;
    table->records[table->recordSize].isEntry = isEntry;
    table->records[table->recordSize].byteCodeForDynamic = byteCodeForDynamic;
    table->recordSize++;
    return true;
}

bool SetLabelIsEntryInTable(SymbolsTable* table, char* label, bool isEntry){
    int labelPos;
    labelPos = isLabelExistsInTable(table, label);
    if (labelPos == LABEL_NOT_EXISTS) {
        return false;
    }

    table->records[labelPos].isEntry = isEntry;
    return true;
}

bool AddExternUsageAddress(SymbolRecord* record, int usedAddress){
    record->externUsageAddresses = (int *) realloc(record->externUsageAddresses, record->externUsages + sizeof(int));
    record->externUsageAddresses[record->externUsages] = usedAddress;
    record->externUsages++;
    return true;
}

void printSymbolTable(SymbolsTable* table){
    int i;
    printf("----------------------------------------------------------------------------------------------------\n");
    printf("label\taddress\tisExternal\tisCommand\tisEntry\tbyteCodeForDynamic\n");\
    for (i = 0; i < table->recordSize ; i++) {
        printf("%s\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", table->records[i].label, table->records[i].address,
               table->records[i].isExternal, table->records[i].isCommand, table->records[i].isEntry, table->records[i].byteCodeForDynamic);
    }
    printf("----------------------------------------------------------------------------------------------------\n");
}