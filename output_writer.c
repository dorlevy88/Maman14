#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "output_writer.h"

#define THREE_BITS_MASK 7  /* this is equal to 111 binary */
#define FILE_OUTPUT_FORMAT "%s\t%s\n"
#define NUM_OF_CHARS_IN_CMD 5
#define NUM_OF_CHARS_IN_ADDRESS 3
#define NUM_OF_CHARS_IN_COUNTERS 2

char* getFilenameNewExtension(char *filename, const char* extension) {
    int dotPos;
    char* newFileName;
    char *dot;

    dot = strrchr(filename, '.');
    if(dot == NULL || dot == filename) return NULL;
    /*  terminate string at the . (dot) location */
    dotPos = (int) (dot - filename);
    newFileName = getNewString(dotPos + strlen(extension));
    strncpy(newFileName, filename, dotPos);
    strcat(newFileName, extension);
    return newFileName;
}

char translateToSpecial8Base(int base8) {
    switch (base8) {
        case 0:
            return '!';
        case 1:
            return '@';
        case 2:
            return '#';
        case 3:
            return '$';
        case 4:
            return '%';
        case 5:
            return '^';
        case 6:
            return '&';
        case 7:
            return '*';
        default:
            /* TODO: error: bug in program */
            return 'X';
    }
}

int convertNumFromBase10toBase8(int base10) {
    int index = 0;
    int res = 0;
    int remainder;
    while (base10 != 0)
    {
        remainder = base10 % 8;
        base10 = base10/8;
        res += remainder * pow(10, index);
        index ++ ;
    }
    return res;
}

char* translateCommandToSpecial8Base(int byte) {
    char* response = getNewString(NUM_OF_CHARS_IN_CMD);
    int i,num;
    for (i = 0; i < NUM_OF_CHARS_IN_CMD; ++i) {
        num = byte & THREE_BITS_MASK; /* Get 3 right most bits */
        byte >>=3;
        response[(NUM_OF_CHARS_IN_CMD - 1)-i] = translateToSpecial8Base(num);
    }
    return response;
}

char* translateAddressToSpecial8Base(int address, int size) {
    int base8 = convertNumFromBase10toBase8(address);
    char* response = getNewString(size);
    int i, num;
    for (i = 1; i <= size ; ++i) {
        num = base8 % 10;
        response[size-i] = translateToSpecial8Base(num);
        base8 /= 10;
    }
    return response;
}

void writeEntOutputFile(SymbolsTable *table, char* filename) {
    FILE *fp;
    int i;
    char* address;
    char* newFileName;
    bool tableHasEntry = false;

    /* check for entry records */
    for (i = 0; i < table->recordSize; ++i) {
        if (table->records[i].isEntry == true) {
            tableHasEntry = true;
            break;
        }
    }
    if (tableHasEntry == false)
        return;

    newFileName = getFilenameNewExtension(filename, ".ent");
    fp = fopen(newFileName, "w");
    printProcessStep("Start writing file", newFileName);
    if (fp == NULL) {
        printInternalError(EER_NEW_FILE_FAILURE, newFileName);
        free(newFileName);
        return;
    }

    for (i = 0; i < table->recordSize; ++i) {
        if (table->records[i].isEntry == true) {
            address = translateAddressToSpecial8Base(table->records[i].address, NUM_OF_CHARS_IN_ADDRESS); /* 3 because it could be up to 1000 */
            fprintf(fp, FILE_OUTPUT_FORMAT, table->records[i].label, address);
            free(address);
        }
    }
    fclose(fp);
    free(newFileName);
    return;
}

void writeExtOutputFile(SymbolsTable* externs, char* filename) {
    FILE *fp;
    int i;
    char* address;
    char* newFileName;

    if(externs->recordSize == 0)
        return;

    newFileName = getFilenameNewExtension(filename, ".ext");
    fp = fopen(newFileName, "w");
    printProcessStep("Start writing file", newFileName);
    if (fp == NULL) {
        printInternalError(EER_NEW_FILE_FAILURE, newFileName);
        free(newFileName);
        return;
    }

    for (i = 0; i < externs->recordSize; ++i) {
        address = translateAddressToSpecial8Base(externs->records[i].address, NUM_OF_CHARS_IN_ADDRESS); /* 3 because it could be up to 1000 */
        fprintf(fp, FILE_OUTPUT_FORMAT, externs->records[i].label, address);
        free(address);
    }
    fclose(fp);
    free(newFileName);
    return;
}

void writeObOutputFile(AssemblyStructure* assembly, char* filename) {
    FILE *fp;
    int i;
    char* address;
    char* command;
    char* translatedCodeArraySize;
    char* translatedDataArraySize;
    int addressCounter;
    char* newFileName;

    newFileName = getFilenameNewExtension(filename, ".ob");
    fp = fopen(newFileName, "w");
    printProcessStep("Start writing file", newFileName);
    if (fp == NULL) {
        printInternalError(EER_NEW_FILE_FAILURE, newFileName);
        free(newFileName);
        return;
    }

    translatedCodeArraySize = translateAddressToSpecial8Base(assembly->codeArray->size, 2); /* TODO: should be also 3? */
    translatedDataArraySize = translateAddressToSpecial8Base(assembly->dataArray->size, 2);
    fprintf(fp, FILE_OUTPUT_FORMAT, translatedCodeArraySize, translatedDataArraySize);
    addressCounter = ASSEMBLY_CODE_START_ADDRESS;
    for (i = 0; i < assembly->codeArray->size; ++i) {
        address = translateAddressToSpecial8Base(addressCounter++, NUM_OF_CHARS_IN_ADDRESS);
        command = translateCommandToSpecial8Base(assembly->codeArray->array[i]);
        fprintf(fp, FILE_OUTPUT_FORMAT, address, command);
        free(address);
        free(command);
    }
    for (i = 0; i < assembly->dataArray->size; ++i) {
        address = translateAddressToSpecial8Base(addressCounter++, NUM_OF_CHARS_IN_ADDRESS);
        command = translateCommandToSpecial8Base(assembly->dataArray->array[i]);
        fprintf(fp, FILE_OUTPUT_FORMAT, address, command);
        free(address);
        free(command);
    }
    fclose(fp);
    free(translatedCodeArraySize);
    free(translatedDataArraySize);
    free(newFileName);
    return;
}

void writeAllOutputFiles(AssemblyStructure *assembly, char *fullFilename) {
    writeEntOutputFile(assembly->symbolsTable, fullFilename);
    writeExtOutputFile(assembly->externs, fullFilename);
    writeObOutputFile(assembly, fullFilename);
}
