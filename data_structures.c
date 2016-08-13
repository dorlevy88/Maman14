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

/**
 * Inserts byte into bytes
 * @param bytes - AssemblyBytes struct
 * @param byte - int
 * @return True if byte was inserted
 */
bool pushByteFromInt(AssemblyBytes *bytes, int byte) {
    if(bytes->size == MAX_ASSEMBLY_BYTES) { /* if there is no more space in array, return false */
        return false;
    }
    bytes->array[bytes->size++] = byte; /* push byte into bytes array and increase size counter by one */
    return true;
}

/**
 * Inserts array of bytes into AssemblyBytes array
 * @param bytes - AssemblyBytes struct
 * @param array - int array
 * @param arraySize - size of int array
 * @return True if array of bytes were inserted
 */
bool pushBytesFromIntArray(AssemblyBytes *bytes, int *array, int arraySize) {
    int i;
    for (i = 0; i < arraySize; ++i) { /* for each int in bytes array, convert int with compliment's two and insert into array */
        if(pushByteFromInt(bytes, convertCompliment2(array[i], DATA_BYTE_SIZE)) == false) {
            return false;
        }
    }
    return true;
}

/**
 * Inserts array of chars into AssemblyBytes array
 * @param bytes - AssemblyBytes struct
 * @param string - int
 * @return True if array of chars were inserted
 */
bool pushBytesFromString(AssemblyBytes *bytes, char *string) {
    int i;
    for (i = 0; i < strlen(string); ++i) { /* for each char in string, push it into AssemblyBytes */
        if(pushByteFromInt(bytes, (int) string[i]) == false) {
            return false;
        }
    }
    if (pushByteFromInt(bytes, (int) '\0') == false) { /* push NULL char into AssemblyBytes after all string was inserted */
        return false;
    }
    return true;
}

/**
 * print AssemblyBytes array for debugging
 * @param bytes
 */
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

/**
 * Initialize memory for a new Assembly Structure
 * @param assembly - Assembly Structure pointer
 * @return Pass if memory allocation succeeded, Fail otherwise
 */
Status initAssemblyStructure(AssemblyStructure** assembly) {
    AssemblyBytes* codeBytes;
    AssemblyBytes* dataBytes;
    SymbolsTable* table;
    SymbolRecord* record;
    SymbolsTable* tableExt;
    SymbolRecord* recordExt;

    (*assembly) = (AssemblyStructure*)malloc(sizeof(AssemblyStructure));
    if ((*assembly) == NULL)
        return Fail;
    memset((*assembly), 0, sizeof(AssemblyStructure));

    codeBytes = (AssemblyBytes*)malloc(sizeof(AssemblyBytes));
    if (codeBytes == NULL)
        return Fail;
    memset(codeBytes, 0, sizeof(AssemblyBytes));
    (*assembly)->codeArray = codeBytes;

    dataBytes = (AssemblyBytes*)malloc(sizeof(AssemblyBytes));
    if (dataBytes == NULL)
        return Fail;
    memset(dataBytes, 0, sizeof(AssemblyBytes));
    (*assembly)->dataArray = dataBytes;


    table = (SymbolsTable*)malloc(sizeof(SymbolsTable));
    if (table == NULL)
        return Fail;
    memset(table, 0, sizeof(SymbolsTable));

    record = (SymbolRecord*)malloc(sizeof(SymbolRecord));
    if (record == NULL)
        return Fail;
    memset(record, 0, sizeof(SymbolRecord));

    table->records = record;
    table->size = 1;
    (*assembly)->symbolsTable = table;

    tableExt = (SymbolsTable*)malloc(sizeof(SymbolsTable));
    if (tableExt == NULL)
        return Fail;
    memset(tableExt, 0, sizeof(SymbolsTable));

    recordExt = (SymbolRecord*)malloc(sizeof(SymbolRecord));
    if (recordExt == NULL)
        return Fail;
    memset(recordExt, 0, sizeof(SymbolRecord));

    tableExt->records = recordExt;
    tableExt->size = 1;
    (*assembly)->externs = tableExt;

    return Pass;
}

/**
 * Clear AssemblyStructure memory
 * @param assembly
 */
void freeAssemblyStructure(AssemblyStructure** assembly){
    int i;
    if ((*assembly)->codeArray != NULL) { /* Clears Code Array */
        if(DEBUG) printf("Clears Code Array\n");
        free((*assembly)->codeArray);
    }
    if ((*assembly)->dataArray != NULL){ /* Clears Data Array */
        if(DEBUG) printf("Clears Data Array\n");
        free((*assembly)->dataArray);
    }

    if ((*assembly)->symbolsTable != NULL){ /* Clears Symbols */
        if(DEBUG) {
            printf("Clears Symbols\n");
            printSymbolTable((*assembly)->symbolsTable);
        }
        for (i = 0; i < (*assembly)->symbolsTable->size; ++i) { /* Clear all records under symbol tables */
            if ((*assembly)->symbolsTable->records[i].label != NULL) {
                if (DEBUG) printf("Clear Record: %s\n", (*assembly)->symbolsTable->records[i].label);
                free((*assembly)->symbolsTable->records[i].label);
            }
        }
        free((*assembly)->symbolsTable->records); /* clear records under symbol table */
        free((*assembly)->symbolsTable); /* clear symbol table */
        if(DEBUG) printf("Cleared Symbols\n");
    }

    if ((*assembly)->externs != NULL){
        if(DEBUG) {
            printf("Clears Externs\n");
            printSymbolTable((*assembly)->externs);
        }
        for (i = 0; i < (*assembly)->externs->size; ++i) { /* clear label records under externs */
            if ((*assembly)->externs->records[i].label != NULL) {
                if (DEBUG) printf("Clear Record: %s\n", (*assembly)->symbolsTable->records[i].label);
                free((*assembly)->externs->records[i].label);
            }
        }
        free((*assembly)->externs->records); /* clear records under externs */
        free((*assembly)->externs); /* clear externs */
        if(DEBUG) printf("Cleared Externs\n");
    }

    free(*assembly); /* clear all assembly memory */
}

/* **********************************************************************
 *
 *     HANDLE SYMBOLS TABLE STRUCTURE
 *
 * **********************************************************************/

/**
 * checks if label exists in symbols table
 * @param table - Symbols table
 * @param label - Label to check existence of
 * @return location of label in symbol table if exists, LABEL_NOT_EXISTS otherwise
 */
int isLabelExistsInTable(SymbolsTable* table, char* label) {
    int i;
    for (i = 0; i < table->recordSize; ++i) { /* for each symbol check if label equals the symbol label */
        if (strcmp(label, table->records[i].label) == 0) {
            return i;
        }
    }
    return LABEL_NOT_EXISTS;
}

/**
 * Add new label into symbols table
 * @param table - symbols table to insert new symbol to
 * @param label - label of symbol
 * @param address - counter of ic / dc
 * @param isExternal - if symbol is external (true / false)
 * @param isCommand - if symbol is command (true / false)
 * @param isEntry - if symbol is entry (true / false)
 * @param byteCodeForDynamic - first char / int if .data or .string
 * @return true if was able to add, false otherwise
 */
bool addNewLabelToTable(SymbolsTable *table, char *label, int address, bool isExternal, bool isCommand, bool isEntry,
                        int byteCodeForDynamic) {
    int labelPos;

    labelPos = isLabelExistsInTable(table, label); /* Check if label already exists in symbols table */
    if(labelPos != LABEL_NOT_EXISTS) { /* if exists, return false */
        return false;
    }
    if (table->recordSize == table->size) { /* Allocate additional space to the array */
        table->records = (SymbolRecord *) realloc(table->records, (table->size + 1) * sizeof(SymbolRecord));
        if (table->records == NULL) { /* Could not reallocate more memory, fatal error, exiting program */
            exit(0);
        }
        memset(&table->records[table->size], 0, sizeof(SymbolRecord));
        table->size++; /* increase table size counter by one */
    }
    table->records[table->recordSize].label = copyString(label);               /* set record label */
    table->records[table->recordSize].address = address;                       /* set record address */
    table->records[table->recordSize].isExternal = isExternal;                 /* set record isExternal flag */
    table->records[table->recordSize].isCommand = isCommand;                   /* set record isCommand flag */
    table->records[table->recordSize].isEntry = isEntry;                       /* set record isEntry flag */
    table->records[table->recordSize].byteCodeForDynamic = byteCodeForDynamic; /* set record byte */
    table->recordSize++; /* increase table records size counter by one */
    return true;
}

/**
 * adds extern label for when it is being used
 * @param table - Symbols table
 * @param label - extern label to add to table
 * @param address - address of extern label to add
 * @return true if was able to add, false otherwise
 */
bool addNewExternToTable(SymbolsTable *table, char *label, int address) {
    if (table->recordSize == table->size) { /* Allocate additional space to the array */
        table->records = (SymbolRecord *) realloc(table->records, (table->size + 1) * sizeof(SymbolRecord));
        if (table->records == NULL) { /* Could not reallocate more memory, fatal error, exiting program */
            exit(0);
        }
        memset(&table->records[table->size], 0, sizeof(SymbolRecord));
        table->size++; /* increase table size counter by one */
    }
    table->records[table->recordSize].label = copyString(label);  /* set record label */
    table->records[table->recordSize].address = address;          /* set record address */
    table->records[table->recordSize].isExternal = false;         /* set record isExternal flag false */
    table->records[table->recordSize].isCommand = false;          /* set record isCommand flag false */
    table->records[table->recordSize].isEntry = false;            /* set record isEntry flag false */
    table->records[table->recordSize].byteCodeForDynamic = false; /* set record byte false */
    table->recordSize++; /* increase table records size counter by one */
    return true;
}

/**
 * set is entry flag for label
 * @param table - Symbols table
 * @param label - Label to set isentry flag for
 * @return location of label in symbol table if exists, LABEL_NOT_EXISTS otherwise
 */
bool setLabelIsEntryInTable(SymbolsTable *table, char *label){
    int labelPos;
    labelPos = isLabelExistsInTable(table, label); /* get label position in symbols table */
    if (labelPos == LABEL_NOT_EXISTS) { /* if label does not exist, return false */
        return false;
    }

    table->records[labelPos].isEntry = true; /* set is entry flag for label */
    return true;
}

/**
 * Print symbols table for debugging
 * @param table - Symbols table
 */
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