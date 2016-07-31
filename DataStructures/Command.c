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
                sizeInIc += 2; //source _ destination address array
            }
            //TODO: Add 3 array at most
            break;
        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case RED:
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
    return sizeInIc;
}

int buildBinaryCommand(FileLine cmdLine) {
    // 101 - num of command operands (2b) - command opcode (4b) - src addressing type (2b) - dest addressing type (2b) - E,R,A (2b)
    int binCmd = 0;

    //const at the beginning of every command & shift 3 bits
    binCmd += (int)0b101;
    binCmd <<= 3;

    //add the num of operand & shift 2 bits
    binCmd += cmdLine.numOfCommandOprands;
    binCmd <<= 2;

    //add command opcode & shift 4 bits
    binCmd += cmdLine.actionType;
    binCmd <<= 4;

    if (cmdLine.numOfCommandOprands >= 1) {
        // at least one operand, add source addressing & shift 2 bits
        binCmd += cmdLine.firstOperValue.addressingType;
        binCmd <<= 2;

        if (cmdLine.numOfCommandOprands == 2) {
            // two operands, add destination addressing
            binCmd += cmdLine.secondOperValue.addressingType;
        }
        // shift 2 bits anyway
        binCmd <<= 2;
    }
    else {
        //in case of no operands shift 4 bits anyway
        binCmd <<= 4;
    }

    //shift 2 bits for the A,R,E - for command it's always zero
    binCmd <<= 2;
    binCmd += 0;

    return binCmd;
}

int convertCompliment2(int num) {
    return ((~num) + 1);
}

int getBitRangefromInt(int num, int minBit, int maxBit) {
    int res = 0;
    int resCount = 0;
    for (int i = minBit; i <= maxBit; ++i) {
        int mask = 1 << i;
        res = (mask & num) << resCount++;
    }
    return res;
}

int buildBinaryData(Operand operand, SymbolsTable table) {
    int binData = 0;
    int labelPos = 0;
    SymbolRecord record;
    switch (operand.addressingType) {
        case NUMBER:
            //add the number
            binData += convertCompliment2(operand.value);
            //shift 2 bits for linker data absolute
            binData <<= 2;
            binData += (int)Absolute;
            break;
        case REGISTER:
            binData += operand.registerNum;
            //shift 6 bits (register address)
            binData <<= 6;
            break;
        case DIRECT:
            labelPos = isLabelExistsInTable(table, operand.label);
            if (labelPos == LABEL_NOT_EXISTS) {
                //TODO: Throw error label should exist in second transition
                return -1;
            }
            record = table.records[labelPos];
            binData += record.address;
            binData <<= 2;
            if (record.isExternal) {
                binData += (int)External;
            }
            else {
                binData += (int)Relocatable;
            }
            break;
        case DYNAMIC:
            labelPos = isLabelExistsInTable(table, operand.label);
            if (labelPos == LABEL_NOT_EXISTS) {
                //TODO: Throw error: label should exist in second transition
                return -1;
            }
            record = table.records[labelPos];
            if (record.isExternal) {
                //TODO: Throw error: Dynamic addressing is not relevant for extern labels
            }
            int num = getBitRangefromInt(record.byteCodeForDynamic, operand.minNum, operand.maxNum);
            num = convertCompliment2(num);
            binData += num;
            binData <<= 2;
            binData += (int)Absolute;
    }
    return binData;
}