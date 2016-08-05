#include <stdlib.h>
#include <printf.h>
#include <math.h>
#include <memory.h>
#include "output_reader.h"

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

int convertNumFromBase10toBase8(int base10) {
    int index = 0;
    int res = 0;
    while (base10 != 0)
    {
        int remainder = base10 % 8;
        base10 = base10/8;
        res += remainder * pow(10, index);
        index ++ ;
    }
    return res;
}

char* translateCommandToSpecial8Base(int byte) {
    char* response = (char*) malloc(sizeof(char) * 5);
    for (int i = 0; i < 5; ++i) {
        int num = byte & 0b111; //Get 3 right most bits
        byte >>=3;
        response[4-i] = translateToSpecial8Base(num);
    }
    return response;
}

char* translateAddressToSpecial8Base(int address) {
    int base8 = convertNumFromBase10toBase8(address);
    char* response = (char*) malloc(sizeof(char) * 3);
    for (int i = 1; i <= 3 ; ++i) { //3 because it could be up to 1000
        int num = base8 % 10;
        response[3-i] = translateToSpecial8Base(num);
        base8 /= 10;
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
            for (int j = 0; j < table->records[i].externUsages; ++j) {
                char* address = translateAddressToSpecial8Base(table->records[i].externUsageAddresses[j]);
                fprintf(fp, "%s %s\n", table->records[i].label, address);
            }
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
        fprintf(fp, "%s %s\n", address, command);
    }
    for (int i = 0; i < assembly->dataArray->size; ++i) {
        char* address = translateAddressToSpecial8Base(addressCounter++);
        char* command = translateCommandToSpecial8Base(assembly->dataArray->array[i]);
        fprintf(fp, "%s %s\n", address, command);
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

    filename = getFilenameNoExtension(filename);
    if (writeExtOutputFile(assembly->symbolsTable, filename) == false) {
        //TODO: Throw error file cannot be created
        //TODO: check if file exists and delete it
        return false;
    }

    filename = getFilenameNoExtension(filename);
    if(writeObOutputFile(assembly, filename) == false) {
        //TODO: Throw error file cannot be created
        //TODO: check if file exists and delete it
        return false;
    }

    return true;
}
