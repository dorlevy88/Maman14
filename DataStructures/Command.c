//
// Created by Dor Levy on 7/10/16.
//

#include <math.h>
#include "Command.h"

#define OPERAND_BYTE_SIZE 13

int buildBinaryCommand(FileLine cmdLine) {
    // 101 - num of command operands (2b) - command opcode (4b) - src addressing type (2b) - dest addressing type (2b) - E,R,A (2b)
    int binCmd = 0;

    //const at the beginning of every command
    binCmd += (int)0b101;

    //shift 2 bits & add the num of operand
    binCmd <<= 2;
    binCmd += cmdLine.numOfCommandOprands;

    //shift 4 bits & add command opcode
    binCmd <<= 4;
    binCmd += cmdLine.actionType;

    if (cmdLine.numOfCommandOprands == 1) {
        // No source operand -shift 2 for source addressing and shift 2 zeros for destination
        binCmd <<= 4;
        binCmd += cmdLine.firstOperValue->addressingType;
    }
    else if (cmdLine.numOfCommandOprands == 2) {
        // shift 2 bits and add source addressing
        binCmd <<= 2;
        binCmd += cmdLine.firstOperValue->addressingType;


        // shift two bits and add destination addressing
        binCmd <<= 2;
        binCmd += cmdLine.secondOperValue->addressingType;
    }
    else {
        //in case of no operands shift 4 bits anyway
        binCmd <<= 4;
    }

    //shift 2 bits for the A,R,E - for command it's always zero
    binCmd <<= 2;

    return binCmd;
}

int convertCompliment2(int num, int size) {
    if (num < 0){
        return (int)(pow(2, size) + num);
    }
    return num;
}

int getBitRangefromInt(int num, int minBit, int maxBit) {
    int res = 0;
    bool isNegative = false;
    for (int i = minBit; i <= maxBit; ++i) {
        int mask = ((1 << i) & num);
        res += mask >> minBit;
        if(i == maxBit && mask != 0) {
            isNegative = true;
        }
    }
    int byteSize = maxBit - minBit + 1;
    if (isNegative){
        res = res - (int)pow(2, byteSize);
    }
    return res;
}

int buildBinaryData(Operand* operand, SymbolsTable* table, bool isDestinationOperand, int cmdAddress) {
    int binData = 0;
    int labelPos = 0;
    SymbolRecord* record;
    switch (operand->addressingType) {
        case NUMBER:
            //add the number
            binData += convertCompliment2(operand->value, OPERAND_BYTE_SIZE);
            //shift 2 bits for linker data absolute
            binData <<= 2;
            binData += (int)Absolute;
            break;
        case REGISTER:
            binData += operand->registerNum;
            if(isDestinationOperand) {
                //shift 2 bits (destination register address)
                binData <<= 2;
            }
            else {
                //shift 8 bits (source register address)
                binData <<= 8;
            }
            break;
        case DIRECT:
            labelPos = isLabelExistsInTable(table, operand->label);
            if (labelPos == LABEL_NOT_EXISTS) {
                //TODO: Throw error label should exist in second transition
                return -1;
            }
            record = &table->records[labelPos];
            binData += record->address;
            binData <<= 2;
            if (record->isExternal) {
                binData += (int)External;
                AddExternUsageAddress(record, cmdAddress);
            }
            else {
                binData += (int)Relocatable;
            }
            break;
        case DYNAMIC:
            labelPos = isLabelExistsInTable(table, operand->label);
            if (labelPos == LABEL_NOT_EXISTS) {
                //TODO: Throw error: label should exist in second transition
                return -1;
            }
            record = &table->records[labelPos];
            if (record->isExternal) {
                //TODO: Throw error: Dynamic addressing is not relevant for extern labels
            }
            int num = getBitRangefromInt(record->byteCodeForDynamic, operand->minNum, operand->maxNum);
            num = convertCompliment2(num, OPERAND_BYTE_SIZE);
            binData += num;
            binData <<= 2;
            binData += (int)Absolute;
    }
    return binData;
}