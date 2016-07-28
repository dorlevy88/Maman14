//
// Created by Or Zamir on 7/17/16.
//

#include <stdbool.h>

#ifndef MAMAN14_ASSEMBLYBYTE_H
#define MAMAN14_ASSEMBLYBYTE_H

#endif //MAMAN14_ASSEMBLYBYTE_H


typedef struct AssemblyByte {

    bool bitarray[15];

    //Byte type: command or addressing

    //Command: new functions ("class") will help decide on addressing types, A.R.E etc...
    //Addressing: type (forward, register, bypass)
} AssemblyByte;