//
// Created by Dor Levy on 7/10/16.
//

#include "CommandGroupTypes.h"
#include "CommandOperationTypes.h"


//page 15

struct operation {
    extern enum GroupType groupType; //number of operands
    int opCode;
};