//
// Created by Or Zamir on 7/17/16.
//

#ifndef MAMAN14_ASSEMBLYBYTE_H
#define MAMAN14_ASSEMBLYBYTE_H

#include <memory.h>
#include <stdbool.h>

#define MAX_ASSEMBLY_BYTES 1000

typedef struct AssemblyBytes {

    int array[MAX_ASSEMBLY_BYTES];
    int size;

} AssemblyBytes;


bool PushByteFromInt(AssemblyBytes* bytes, int byte);

bool PushBytesFromIntArray(AssemblyBytes* bytes, int* array, int arraySize);

bool PushBytesFromString(AssemblyBytes* bytes, char* string);

#endif //MAMAN14_ASSEMBLYBYTE_H