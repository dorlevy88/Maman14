//
// Created by Or Zamir on 7/31/16.
//

#include "OutputFiles.h"

//Trim int to 16Bit for writing
#define to16bit(n) (n)>=0? n : ((n)^-65536)


bool writeEntOutpuFile(SymbolsTable* table) {
    //TODO: implement
}

bool writeExtOutpuFile(SymbolsTable* table) {
    //TODO: implement
}

bool writeObOutpuFile(SymbolsTable* table) {
    //TODO: implement
}

char* transtaleToSpecial8Base(int threeBytes[3]) {

}

bool WriteAllOutpuFiles(AssemblyStructure* assembly) {
    //TODO: implement

    writeEntOutpuFile(assembly->symbolsTable);

    writeExtOutpuFile(assembly->symbolsTable);

    writeObOutpuFile(assembly->symbolsTable);
}
