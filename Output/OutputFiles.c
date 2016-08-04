//
// Created by Or Zamir on 7/31/16.
//

#include <stdlib.h>
#include <printf.h>
#include "OutputFiles.h"

//Trim int to 16Bit for writing
#define to16bit(n) (n)>=0? n : ((n)^-65536)





bool writeEntOutputFile(SymbolsTable *table) {
    //TODO: implement
}

bool writeExtOutputFile(SymbolsTable *table) {
    //TODO: implement
}

bool writeObOutputFile(AssemblyBytes* code, AssemblyBytes* data, int ic) {
    for (int i = 0; i < code->size; ++i) {
        printf("");
    }
}

char translateToSpecial8Base(int base10) {
    switch (base10) {
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
            //bug
            return NULL;
    }
}

char* transtaleCommandToSpecial8Base(int byte) {
    char* response = (char*) malloc(sizeof(char) * 5);
    for (int i = 0; i < 5; ++i) {
        int num = byte & 0b111; //Get 3 right most bits
        response[i] = translateToSpecial8Base(num);
    }
    return response;
}

char* transtaleAddressToSpecial8Base(int address) {
    char* response = (char*) malloc(sizeof(char) * 3);
    for (int i = 1; i <= 3 ; ++i) { //3 because it could be up to 1000
        int num = address % (i * 10);
        response[i] = translateToSpecial8Base(num);
    }
    return response;
}

bool WriteAllOutpuFiles(AssemblyStructure* assembly) {
    //TODO: implement

    writeEntOutputFile(assembly->symbolsTable);

    writeExtOutputFile(assembly->symbolsTable);

    writeObOutputFile(assembly->symbolsTable);
}
