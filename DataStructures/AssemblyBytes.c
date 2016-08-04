//
// Created by Or Zamir on 7/30/16.
//

#include <printf.h>
#include <math.h>
#include "AssemblyBytes.h"
#include "Command.h"


#define DATA_BYTE_SIZE 15


bool PushByteFromInt(AssemblyBytes* bytes, int byte) {
    if(bytes->size == MAX_ASSEMBLY_BYTES) {
        return false;
    }
    bytes->array[bytes->size++] = byte;
    return true;
}

bool PushBytesFromIntArray(AssemblyBytes* bytes, int* array, int arraySize) {
    for (int i = 0; i < arraySize; ++i) {
        if(PushByteFromInt(bytes, convertCompliment2(array[i], DATA_BYTE_SIZE)) == false) {
            return false;
        }
    }
    return true;
}

bool PushBytesFromString(AssemblyBytes* bytes, char* string) {
    for (int i = 0; i < strlen(string); ++i) {
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
    printf("----------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < bytes->size; i++) {
        printf("%d\t%015ld\n", i, decimalToBinary(bytes->array[i]));
    }
    printf("----------------------------------------------------------------------------------------------------\n");
}