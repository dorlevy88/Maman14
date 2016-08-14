#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "output_writer.h"

#define THREE_BITS_MASK 7  /* this is equal to 111 binary */
#define FILE_OUTPUT_FORMAT "%s\t%s\n"
#define NUM_OF_CHARS_IN_CMD 5
#define NUM_OF_CHARS_IN_ADDRESS 3
#define NUM_OF_CHARS_IN_COUNTERS 2

/**
 * replace the extension on filename from what it was to extension
 * @param filename - full filename (*.*)
 * @param extension - New extension for file name
 * @return string of new file name
 */
char* getFilenameNewExtension(char *filename, const char* extension) {
    int dotPos;
    char* newFileName;
    char *dot;

    dot = strrchr(filename, '.'); /* find the dot in the string */
    if(dot == NULL || dot == filename) return NULL; /* if couldn't find a dot or file starts with a dot */
    dotPos = (int) (dot - filename);  /* terminate string at the . (dot) location */
    newFileName = getNewString(dotPos + (int)strlen(extension)); /* Get a memory allocation for new filename */
    strncpy(newFileName, filename, dotPos); /* get file name base */
    strcat(newFileName, extension); /* concatenate to file base the new extension */
    return newFileName;
}

/**
 * switch integer in base 8 to char in base 8 special
 * @param base8 - int represented in base 8
 * @return base 8 special char
 */
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

/**
 * Converts base 10 integer to base 8 integer
 * @param base10 - base 10 integer
 * @return base 8 integer
 */
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

/**
 * translates an integer that represents a 15 bits command into special 8 base
 * @param byte - integer representss a 15 bits command
 * @return 5 char string represnting the byte in special 8 base
 */
char* translateCommandToSpecial8Base(int byte) {
    char* response = getNewString(NUM_OF_CHARS_IN_CMD);
    int i,num;
    for (i = 0; i < NUM_OF_CHARS_IN_CMD; ++i) {
        num = byte & THREE_BITS_MASK; /* Get 3 right most bits */
        byte >>=3; /* remove 3 rightmost bits */
        response[(NUM_OF_CHARS_IN_CMD - 1)-i] = translateToSpecial8Base(num); /* insert into response, starting from the inner most object in array (4,3,2,1,0) the base 8 char */
    }
    return response;
}

/**
 * translates an adress (100,102 ...) into special 8 base string
 * @param address - integer represnting address in base 10
 * @param size -
 * @return
 */
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

/**
 *
 * @param table
 * @param filename
 */
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

/**
 *
 * @param externs
 * @param filename
 */
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

/**
 *
 * @param assembly
 * @param filename
 */
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

/**
 * write all files - .ob / .ent / .ext
 * @param assembly - AssemblyStructure to write
 * @param fullFilename - Base file name to write (same as the original .as file)
 */
void writeAllOutputFiles(AssemblyStructure *assembly, char *fullFilename) {
    writeEntOutputFile(assembly->symbolsTable, fullFilename);
    writeExtOutputFile(assembly->externs, fullFilename);
    writeObOutputFile(assembly, fullFilename);
}
