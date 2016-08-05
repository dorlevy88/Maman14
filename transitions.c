#include <math.h>
#include <stddef.h>
#include <memory.h>
#include "transitions.h"

#define CMD_BYTE_BEGIN 0b101

int buildBinaryCommand(FileLine cmdLine) {
    /*  101 - num of command operands (2b) - command opcode (4b) - src addressing type (2b) - dest addressing type (2b) - E,R,A (2b) */
    int binCmd = 0;

    /* const at the beginning of every command */
    binCmd += CMD_BYTE_BEGIN;

    /* shift 2 bits & add the num of operand */
    binCmd <<= 2;
    binCmd += cmdLine.numOfCommandOprands;

    /* shift 4 bits & add command opcode */
    binCmd <<= 4;
    binCmd += cmdLine.actionType;

    if (cmdLine.numOfCommandOprands == 1) {
        /*  No source operand -shift 2 for source addressing and shift 2 zeros for destination */
        binCmd <<= 4;
        binCmd += cmdLine.firstOperValue->addressingType;
    }
    else if (cmdLine.numOfCommandOprands == 2) {
        /*  shift 2 bits and add source addressing */
        binCmd <<= 2;
        binCmd += cmdLine.firstOperValue->addressingType;


        /*  shift two bits and add destination addressing */
        binCmd <<= 2;
        binCmd += cmdLine.secondOperValue->addressingType;
    }
    else {
        /* in case of no operands shift 4 bits anyway */
        binCmd <<= 4;
    }

    /* shift 2 bits for the A,R,E - for command it's always zero */
    binCmd <<= 2;

    return binCmd;
}

int getBitRangefromInt(int num, int minBit, int maxBit) {
    int res = 0;
    int i, mask, byteSize;
    bool isNegative = false;
    for (i = minBit; i <= maxBit; ++i) {
        mask = ((1 << i) & num);
        res += mask >> minBit;
        if(i == maxBit && mask != 0) {
            isNegative = true;
        }
    }
    byteSize = maxBit - minBit + 1;
    if (isNegative){
        res = res - (int)pow(2, byteSize);
    }
    return res;
}

int buildBinaryData(Operand* operand, SymbolsTable* table, bool isDestinationOperand, int cmdAddress) {
    int binData = 0;
    int labelPos = 0;
    SymbolRecord* record;
    int num;
    switch (operand->addressingType) {
        case NUMBER:
            /* add the number */
            binData += ConvertCompliment2(operand->value, OPERAND_BYTE_SIZE);
            /* shift 2 bits for linker data absolute */
            binData <<= 2;
            binData += (int)Absolute;
            break;
        case REGISTER:
            binData += operand->registerNum;
            if(isDestinationOperand) {
                /* shift 2 bits (destination register address) */
                binData <<= 2;
            }
            else {
                /* shift 8 bits (source register address) */
                binData <<= 8;
            }
            break;
        case DIRECT:
            labelPos = isLabelExistsInTable(table, operand->label);
            if (labelPos == LABEL_NOT_EXISTS) {
                /* TODO: Throw error label should exist in second transition */
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
                /* TODO: Throw error: label should exist in second transition */
                return -1;
            }
            record = &table->records[labelPos];
            if (record->isExternal) {
                /* TODO: Throw error: Dynamic addressing is not relevant for extern labels */
            }
            num = getBitRangefromInt(record->byteCodeForDynamic, operand->minNum, operand->maxNum);
            num = ConvertCompliment2(num, OPERAND_BYTE_SIZE);
            binData += num;
            binData <<= 2;
            binData += (int)Absolute;
    }
    return binData;
}


void UpdateSymbolsTableDataAddresses(SymbolsTable* table, int ic) {
    int i;
    for (i = 0; i < table->recordSize; ++i) {
        if (table->records[i].isCommand == false && table->records[i].isExternal == false) { /* update only .data\.string types */
            table->records[i].address += ic;
        }
    }
}

int getCommandSize(FileLine* line) {
    int sizeInIc = 0;
    switch (line->actionType) {
        case MOV:
        case CMP:
        case ADD:
        case SUB:
        case LEA:
            sizeInIc++; /* command byte */
            if (line->firstOperValue->addressingType == REGISTER &&
                line->secondOperValue->addressingType == REGISTER) {
                sizeInIc++; /* if both register addressing -> share a byte */
            }
            else {
                sizeInIc += 2; /* source _ destination address array */
            }
            /* TODO: Add 3 array at most */
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
            sizeInIc += 2; /* command byte + adresss */
            break;
        case RTS:
        case STOP:
            sizeInIc++; /* only command byte */
            break;
        default:
            /* BUG in the algorithm should not go here */
            break;
    }
    return sizeInIc;
}

bool RunFirstTransition(FileContent* fileContent, AssemblyStructure* assembly) {
    /*
        1. int ic = 0, dc = 0;
        2. Read line
        3. check is symbol exists in the first field
        4. Rise isLabelExists flag - if it exists
        5. if it is data storage order? (.data or .string) if not go to step 8
        6. if there is a symbol add it to the symbols table with the value of dc counter (if it exists in the table throw error)
        7. identify the data type and store it in memory
        7.1. update the dc counter according to the data size return to step 2(!)
        8. if the order is .extern or .entry (if not go to step 11)
        9. is it .extern order
        9.1 add it to the EXTERN symbols table with no value
        10. return to step 2
        11. if there is a label add it to the symbols table with the value of ic counter (if it exists in the table throw error)
        12. search the commands table if not exists throw unknown command code error
        13. Check command operand type and calculate the command memory size (L)
        13.1 You can produce the order code here
        14. Add to ic the value of ic + the calculated L value (ic += L)
        15. go back to step 2
    */
    int i, calcCommandSize;
    assembly->ic = assembly->startAddress;
    assembly->dc = 0;

    for (i=0; i < fileContent->size; i++){ /* For every line in file */
        FileLine line = fileContent->line[i];


        bool isLabelExists = false;

        if (line.label != NULL) { /* If Label Exists for line */
            isLabelExists = true;
        }

        /* Handle Data Storage Symbols */
        if (line.actionType == DATA || line.actionType == STRING) {
            /* Steps 6 ,7 and 7.1 */

            /* Push into data array */
            int calcDataSize = 0;
            int firstByte = 0;
            bool isMemAllocOk;
            if (line.actionType == DATA ) {
                calcDataSize = line.firstOperValue->dataSize;
                firstByte = line.firstOperValue->data[0];
                isMemAllocOk = PushBytesFromIntArray(assembly->dataArray, line.firstOperValue->data, line.firstOperValue->dataSize);
            }
            else {
                calcDataSize = (int)strlen(line.firstOperValue->string) + 1;
                firstByte = line.firstOperValue->string[0];
                isMemAllocOk = PushBytesFromString(assembly->dataArray, line.firstOperValue->string);
            }

            if(isMemAllocOk == false) {
                PrintCompileError(ERR_RAM_OVERFLOW, "Data Array", line.lineNumber);
                return false;
            }
            if (isLabelExists) {
                if (AddNewLabelToTable(assembly->symbolsTable, line.label, assembly->dc, false, false, false,
                                       firstByte) == false) {
                    PrintCompileError(ERR_LABEL_DEFINED_TWICE, line.label, line.lineNumber);
                    return false;
                }
            }
            assembly->dc += calcDataSize;
        }
            /* Handle External Symbols */
        else if (line.actionType == EXTERN || line.actionType == ENTRY) {
            if (isLabelExists) {
                PrintCompileWarning(WARN_LABEL_IN_BAD_LOCATION, line.label, line.lineNumber);
            }
            /* Steps 9, 9.1, 10 */
            if (line.actionType == EXTERN) {
                AddNewLabelToTable(assembly->symbolsTable, line.firstOperValue->entryOrExtern, 0, true, false, false, 0);
                /* TODO: Should we send error if label exists in table? */
            }
        }
        else {/*  handle command line */

            /* Handle Command labels */
            if (isLabelExists) {
                /* Step 11 */
                int binCommandForDynamicAddressing = buildBinaryCommand(line);
                if (AddNewLabelToTable(assembly->symbolsTable, line.label, assembly->ic, false, true, false,
                                       binCommandForDynamicAddressing) == false) {
                    PrintCompileError(ERR_LABEL_DEFINED_TWICE, line.label, line.lineNumber);
                    return false;
                }
            }

            /* Steps 12, 13, 13.1, 14 */
            /* handle Command size */
            calcCommandSize = getCommandSize(&line);
            /* Step 14 */
            assembly->ic += calcCommandSize;
        }
    }

    if (assembly->ic + assembly->dc >= MAX_CPU_MEMORY) {
        /* TODO: Throw error program is larger than CPU memory */
    }

    UpdateSymbolsTableDataAddresses(assembly->symbolsTable, assembly->ic);
    return true;
}

bool RunSecondTransition(FileContent* fileContent, AssemblyStructure* assembly) {

    /* 1. ic = 0
    2. read word if done go to step 11
    3. ignore label on the beginning of line
    4. is it .data/.string go to step 2 else continue
    5. is it .extern or .entry (if not go to step 7)
    6. identify the order
    6.1. act accordingly
    6.2 if it's .entry order add the entry symbols and return to step 2
    7. estimate the operands look at the command table and switch to the correct command code
    8. store the operands in the next byte
    8.1 if it a symbol find a match in the symbols table
    8.2 calculate addressing & their matching codes
    9. ic += L
    10. return to step 2
    11. save to a different file:
    11.1 size of the program
    11.2 size of the data
    11.3 extern symbols table
    11.4 symbols table with entry points marks */
    int i;
    int tmpIc = assembly->ic;
    assembly->ic = assembly->startAddress;

    for (i=0; i < fileContent->size; i++) { /* For every line in file */

/*         printf("%s", fileContent->line[i].originalLine); */
        FileLine line = fileContent->line[i];


        /* TODO:Debug

        printSymbolTable(assembly->symbolsTable);
        printAssemblyByte(assembly->codeArray);
        printAssemblyByte(assembly->dataArray);

         */

        /* Step: 4 */
        if (line.actionType == DATA || line.actionType == STRING) {
            /* do nothing */
        }
            /* Step: 5 */
        else if (line.actionType == EXTERN || line.actionType == ENTRY) {
            if (line.actionType == ENTRY) {
                if (SetLabelIsEntryInTable(assembly->symbolsTable, line.firstOperValue->entryOrExtern, true) == false) {
                    PrintCompileError(ERR_LABEL_NOT_DEFINED, line.firstOperValue->entryOrExtern, line.lineNumber);
                    return false;
                }
            }
        }
            /* handle command */
        else{
            /*  101 - num od command operands (2b) - command opcode (4b) - src addressing type (2b) - dest addressing type (2b) - E,R,A (2b) */
            int binCmd = buildBinaryCommand(line);
            bool isMemAllocOk = PushByteFromInt(assembly->codeArray, binCmd);
            assembly->ic++;
            /*  0000000000000-00 (data 13b - E,R,A (2b)) */
            /* Building Code Array */
            if (line.numOfCommandOprands == 1){
                int binData = buildBinaryData(line.firstOperValue, assembly->symbolsTable, true, assembly->ic);
                isMemAllocOk &= PushByteFromInt(assembly->codeArray, binData);
                assembly->ic++;
            }
            else if (line.numOfCommandOprands == 2) {
                int firstBinData = buildBinaryData(line.firstOperValue, assembly->symbolsTable, false, assembly->ic);
                int secondBinData = buildBinaryData(line.secondOperValue, assembly->symbolsTable, true, assembly->ic+1);
                if (line.firstOperValue->addressingType == REGISTER && line.secondOperValue->addressingType == REGISTER){
                    isMemAllocOk &= PushByteFromInt(assembly->codeArray, firstBinData + secondBinData);
                    assembly->ic++;
                }
                else{
                    isMemAllocOk &= PushByteFromInt(assembly->codeArray, firstBinData);
                    isMemAllocOk &= PushByteFromInt(assembly->codeArray, secondBinData);
                    assembly->ic+=2;
                }
            }

            if(isMemAllocOk == false){
                PrintCompileError(ERR_RAM_OVERFLOW, "Code Array", line.lineNumber);
                return false;
            }
        }

    }

    if (assembly->ic != tmpIc) {
        /* TODO: BUG: something went wrong with the algorithm */
    }

    /* TODO: check not more than 1000 bytes of code + data */
    return true;
    /* Done and ready to save! */
}