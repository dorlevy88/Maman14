//
// Created by Or Zamir on 7/31/16.
//

#include <stdlib.h>
#include <printf.h>
#include "OutputFiles.h"

//TODO: Trim int to 16Bit for writing
#define to16bit(n) (n)>=0? n : ((n)^-65536)

char* getFilenameNoExtension(char *filename) {
    char *dot = strrchr(filename, '.');
    if(dot == NULL || dot == filename) return "";
    dot[0] = '\0'; // terminate string at the . (dot) location
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
            //TODO: error: bug in program
            return 'X';
    }
}

char* translateCommandToSpecial8Base(int byte) {
    char* response = (char*) malloc(sizeof(char) * 6);
    for (int i = 0; i < 5; ++i) {
        int num = byte & 0b111; //Get 3 right most bits
        response[i] = translateToSpecial8Base(num);
    }
    return response;
}

char* translateAddressToSpecial8Base(int address) {
    char* response = (char*) malloc(sizeof(char) * 4);
    for (int i = 1; i <= 3 ; ++i) { //3 because it could be up to 1000
        int num = address % (i * 10);
        response[i-1] = translateToSpecial8Base(num);
    }
    return response;
}

bool writeEntOutputFile(SymbolsTable *table, char* filename) {
    FILE *fp = fopen(strcat(filename, ".ent"), "w");
    if (fp == NULL) {
        return false;
    }

    for (int i = 0; i < table->recordSize; ++i) {
        if (table->records[i].isEntry == true) {
            char* address = translateAddressToSpecial8Base(table->records[i].address);
            fprintf(fp, "%s %s\n", table->records[i].label, address);
        }
    }
    fclose(fp);
    return true;
}

bool writeExtOutputFile(SymbolsTable *table, char* filename) {
    FILE *fp = fopen(strcat(filename, ".ext"), "w");
    if (fp == NULL) {
        return false;
    }

    for (int i = 0; i < table->recordSize; ++i) {
        if (table->records[i].isExternal == true) {
            char* address = translateAddressToSpecial8Base(table->records[i].address);
            fprintf(fp, "%s %s\n", table->records[i].label, address);
        }
    }
    fclose(fp);
    return true;
}

bool writeObOutputFile(AssemblyStructure* assembly, char* filename) {
    FILE *fp = fopen(strcat(filename, ".ob"), "w");
    if (fp == NULL) {
        return false;
    }

    int addressCounter = assembly->startAddress;
    for (int i = 0; i < assembly->codeArray->size; ++i) {
        char* address = translateAddressToSpecial8Base(addressCounter++);
        char* command = translateCommandToSpecial8Base(assembly->codeArray->array[i]);
        fprintf(fp, "%s %s", address, command);
    }
    for (int i = 0; i < assembly->dataArray->size; ++i) {
        char* address = translateAddressToSpecial8Base(addressCounter++);
        char* command = translateCommandToSpecial8Base(assembly->dataArray->array[i]);
        fprintf(fp, "%s %s", address, command);
    }
    fclose(fp);
    return true;
}

bool WriteAllOutputFiles(AssemblyStructure* assembly, char* fullFilename) {
    //TODO: implement

    char* filename = getFilenameNoExtension(fullFilename);

    if (writeEntOutputFile(assembly->symbolsTable, filename) == false) {
        //TODO: Throw error file cannot be created
        //TODO: check if file exists and delete it
        return false;
    }

    if (writeExtOutputFile(assembly->symbolsTable, filename) == false) {
        //TODO: Throw error file cannot be created
        //TODO: check if file exists and delete it
        return false;
    }

    if(writeObOutputFile(assembly, filename) == false) {
        //TODO: Throw error file cannot be created
        //TODO: check if file exists and delete it
        return false;
    }

    return true;
}
