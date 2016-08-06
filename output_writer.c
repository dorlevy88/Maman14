#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "output_writer.h"

#define THREE_BITS_MASK 7  /* this is equal to 111 binary */


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
    char* response = (char*) malloc(sizeof(char) * 6);
    int i,num;
    for (i = 0; i < 5; ++i) {
        num = byte & THREE_BITS_MASK; /* Get 3 right most bits */
        byte >>=3;
        response[4-i] = translateToSpecial8Base(num);
    }
    response[5] = '\0';
    return response;
}

char* translateAddressToSpecial8Base(int address, int size) {
    int base8 = convertNumFromBase10toBase8(address);
    char* response = (char*) malloc(sizeof(char) * (size + 1));
    int i, num;
    for (i = 1; i <= size ; ++i) { /* 3 because it could be up to 1000 */
        num = base8 % 10;
        response[size-i] = translateToSpecial8Base(num);
        base8 /= 10;
    }
    response[size] = '\0';
    return response;
}

bool writeEntOutputFile(SymbolsTable *table, char* filename) {
    FILE *fp;
    int i;
    char* address;
    char* newFileName;

    if(table->recordSize == 0)
        return true;

    newFileName = getFilenameNewExtension(filename, ".ent");
    fp = fopen(newFileName, "w");
    PrintProcessStep("Start writing file", newFileName);
    if (fp == NULL) {
        return false;
    }

    for (i = 0; i < table->recordSize; ++i) {
        if (table->records[i].isEntry == true) {
            address = translateAddressToSpecial8Base(table->records[i].address, 3);
            fprintf(fp, "%s %s\n", table->records[i].label, address);
            free(address);
        }
    }
    fclose(fp);
    free(newFileName);
    return true;
}

bool writeExtOutputFile(SymbolsTable* externs, char* filename) {
    FILE *fp;
    int i;
    char* address;
    char* newFileName;

    if(externs->recordSize == 0)
        return true;

    newFileName = getFilenameNewExtension(filename, ".ext");
    fp = fopen(newFileName, "w");
    PrintProcessStep("Start writing file", newFileName);
    if (fp == NULL) {
        return false;
    }

    for (i = 0; i < externs->recordSize; ++i) {
        address = translateAddressToSpecial8Base(externs->records[i].address, 3);
        fprintf(fp, "%s %s\n", externs->records[i].label, address);
        free(address);
    }
    fclose(fp);
    free(newFileName);
    return true;
}

bool writeObOutputFile(AssemblyStructure* assembly, char* filename) {
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
    PrintProcessStep("Start writing file", newFileName);
    if (fp == NULL) {
        return false;
    }

    translatedCodeArraySize = translateAddressToSpecial8Base(assembly->codeArray->size, 2);
    translatedDataArraySize = translateAddressToSpecial8Base(assembly->dataArray->size, 2);
    fprintf(fp, "%s %s\n", translatedCodeArraySize, translatedDataArraySize);
    addressCounter = assembly->startAddress;
    for (i = 0; i < assembly->codeArray->size; ++i) {
        address = translateAddressToSpecial8Base(addressCounter++, 3);
        command = translateCommandToSpecial8Base(assembly->codeArray->array[i]);
        fprintf(fp, "%s %s\n", address, command);
        free(address);
        free(command);
    }
    for (i = 0; i < assembly->dataArray->size; ++i) {
        address = translateAddressToSpecial8Base(addressCounter++, 3);
        command = translateCommandToSpecial8Base(assembly->dataArray->array[i]);
        fprintf(fp, "%s %s\n", address, command);
        free(address);
        free(command);
    }
    fclose(fp);
    free(translatedCodeArraySize);
    free(translatedDataArraySize);
    free(newFileName);
    return true;
}

bool WriteAllOutputFiles(AssemblyStructure* assembly, char* fullFilename) {
    if (writeEntOutputFile(assembly->symbolsTable, fullFilename) == false) {
        /* TODO: Throw error file cannot be created */
        /* TODO: check if file exists and delete it */
        return false;
    }
    PrintProcessStep("End writing file", fullFilename);

    if (writeExtOutputFile(assembly->externs, fullFilename) == false) {
        /* TODO: Throw error file cannot be created */
        /* TODO: check if file exists and delete it */
        return false;
    }
    PrintProcessStep("End writing file", fullFilename);

    if(writeObOutputFile(assembly, fullFilename) == false) {
        /* TODO: Throw error file cannot be created */
        /* TODO: check if file exists and delete it */
        return false;
    }
    PrintProcessStep("End writing file", fullFilename);
    return true;
}
