#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "output_writer.h"

#define THREE_BITS_MASK 7  /* this is equal to 111 binary */


char* getFilenameNoExtension(char *filename) {
    char *dot = strrchr(filename, '.');
    if(dot == NULL || dot == filename) return "";
    dot[0] = '\0'; /*  terminate string at the . (dot) location */
    return filename;
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
    char* response = (char*) malloc(sizeof(char) * 5);
    int i,num;
    for (i = 0; i < 5; ++i) {
        num = byte & THREE_BITS_MASK; /* Get 3 right most bits */
        byte >>=3;
        response[4-i] = translateToSpecial8Base(num);
    }
    return response;
}

char* translateAddressToSpecial8Base(int address, int size) {
    int base8 = convertNumFromBase10toBase8(address);
    char* response = (char*) malloc(sizeof(char) * size);
    int i, num;
    for (i = 1; i <= size ; ++i) { /* 3 because it could be up to 1000 */
        num = base8 % 10;
        response[size-i] = translateToSpecial8Base(num);
        base8 /= 10;
    }
    return response;
}

bool writeEntOutputFile(SymbolsTable *table, char* filename) {
    FILE *fp;
    int i;
    char* address;
    fp = fopen(strcat(filename, ".ent"), "w");
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
    return true;
}

bool writeExtOutputFile(SymbolsTable *table, char* filename) {
    FILE *fp;
    int i,j;
    char* address;
    fp = fopen(strcat(filename, ".ext"), "w");
    if (fp == NULL) {
        return false;
    }

    for (i = 0; i < table->recordSize; ++i) {
        if (table->records[i].isExternal == true) {
            for (j = 0; j < table->records[i].externUsages; ++j) {
                address = translateAddressToSpecial8Base(table->records[i].externUsageAddresses[j], 3);
                fprintf(fp, "%s %s\n", table->records[i].label, address);
                free(address);
            }
        }
    }
    fclose(fp);
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
    fp = fopen(strcat(filename, ".ob"), "w");
    if (fp == NULL) {
        return false;
    }

    translatedCodeArraySize = translateAddressToSpecial8Base(assembly->codeArray->size, 2);
    translatedDataArraySize = translateAddressToSpecial8Base(assembly->dataArray->size, 2);
    fprintf(fp, "%s %s\n", translatedCodeArraySize, translatedDataArraySize);
    free(translatedCodeArraySize);
    free(translatedDataArraySize);
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
    return true;
}

bool WriteAllOutputFiles(AssemblyStructure* assembly, char* fullFilename) {
    char* filename = (char*)malloc(strlen(fullFilename));
    strcpy(filename, fullFilename);
    filename = getFilenameNoExtension(filename);
    if (writeEntOutputFile(assembly->symbolsTable, filename) == false) {
        /* TODO: Throw error file cannot be created */
        /* TODO: check if file exists and delete it */
        return false;
    }
    filename = getFilenameNoExtension(filename);
    if (writeExtOutputFile(assembly->symbolsTable, filename) == false) {
        /* TODO: Throw error file cannot be created */
        /* TODO: check if file exists and delete it */
        return false;
    }
    filename = getFilenameNoExtension(filename);
    if(writeObOutputFile(assembly, filename) == false) {
        /* TODO: Throw error file cannot be created */
        /* TODO: check if file exists and delete it */
        return false;
    }
    free(filename);
    return true;
}
