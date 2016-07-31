//
// Created by Or Zamir on 7/17/16.
//

#ifndef MAMAN14_ASSEMBLYBYTE_H
#define MAMAN14_ASSEMBLYBYTE_H

typedef struct AssemblyBytes {

    int* bitarray[1000];
    int size;

} AssemblyBytes;


void PushByteFromInt(AssemblyBytes bytes, int byte);

void PushBytesFromIntArray(AssemblyBytes bytes, int* array, int arraySize);

void PushBytesFromString(AssemblyBytes bytes, char* string);

#endif //MAMAN14_ASSEMBLYBYTE_H