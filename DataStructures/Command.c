//
// Created by Dor Levy on 7/10/16.
//

#include "Command.h"

int getCommandSize(ActionTypes action, OperandAddressingType first, OperandAddressingType second) {
    int sizeInIc = 0;
    switch (action) {
        case MOV:
        case CMP:
        case ADD:
        case SUB:
        case LEA:
            sizeInIc++; //command byte
            if (first == REGISTER && second == REGISTER) {
                sizeInIc++; //if both register addressing -> share a byte
            }
            else {
                sizeInIc += 2; //source _ destination address bytes
            }
            //TODO: Add 3 bytes at most
            break;
        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case PRN:
        case JSR:
            sizeInIc += 2; //command byte + adresss
            break;
        case RTS:
        case STOP:
            sizeInIc++; //only command byte
            break;
        default:
            //BUG in the algorithm should not go here
            break;
    }

}