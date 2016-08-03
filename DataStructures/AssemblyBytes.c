//
// Created by Or Zamir on 7/30/16.
//

#include "AssemblyBytes.h"


bool PushByteFromInt(AssemblyBytes* bytes, int byte) {
    if(bytes->size == MAX_ASSEMBLY_BYTES) {
        return false;
    }
    bytes->array[bytes->size++] = byte;
    return true;
}

bool PushBytesFromIntArray(AssemblyBytes* bytes, int* array, int arraySize) {
    for (int i = 0; i < arraySize; ++i) {
        if(PushByteFromInt(bytes, array[i]) == false) {
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