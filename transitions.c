#include <math.h>
#include <stddef.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include "transitions.h"

#define CMD_BYTE_BEGIN 5 /* equal to 0b101 in binary */
#define OPERAND_BYTE_SIZE 13
#define MAX_CPU_MEMORY 1000

/**
 * Create a 15 bit array from a command line depends on the internal line specifications
 * @param cmdLine - Line to translate to 15 bits
 * @return int represnting a 15 bit array
 */
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

/**
 * Parses a range of bits out of a bit array
 * @param num - Int representing a 15 bits array
 * @param minBit - Bit to start taking data from
 * @param maxBit - Bit to End taking data from
 * @return int represnting the sub bit array
 */
int getBitRangeFromInt(int num, int minBit, int maxBit) {
    int res = 0;
    int i, mask, byteSize;
    bool isNegative = false;

    for (i = minBit; i <= maxBit; ++i) {
        mask = ((1 << i) & num); /* create a bit in the place of i and get therefore get the bit in the i place from num */
        res += mask >> minBit; /* move the bit found to be start from 0 and not from minBit */
        if(i == maxBit && mask != 0) { /* check if last bit is 0 or 1 (+/-) to know if needed to move it with complement's two */
            isNegative = true;
        }
    }
    byteSize = maxBit - minBit + 1; /* amount of bits in sub bit array */
    if (isNegative){
        res = res - (int)pow(2, byteSize);  /* translate to unsigned int from signed int - complement's two */
    }
    return res;
}

/**
 * 8.1 if it a symbol find a match in the symbols table
 * 8.2 calculate addressing & their matching codes
 * @param binData
 * @param operand
 * @param table
 * @param externs
 * @param isDestinationOperand
 * @param cmdAddress
 * @return
 */
char* buildBinaryData(int* binData, Operand* operand, SymbolsTable* table, SymbolsTable* externs, bool isDestinationOperand, int cmdAddress) {
    int labelPos = 0;
    SymbolRecord* record;
    int num;
    switch (operand->addressingType) {
        case NUMBER:
            /* add the number */
            *binData += convertCompliment2(operand->value, OPERAND_BYTE_SIZE);
            /* shift 2 bits for linker data absolute */
            *binData <<= 2;
            *binData += (int)Absolute;
            break;
        case REGISTER:
            *binData += operand->registerNum;
            if(isDestinationOperand) {
                /* shift 2 bits (destination register address) */
                *binData <<= 2;
            }
            else {
                /* shift 8 bits (source register address) */
                *binData <<= 8;
            }
            break;
        case DIRECT:
            labelPos = isLabelExistsInTable(table, operand->label);
            if (labelPos == LABEL_NOT_EXISTS) {
                return errMessage(ERR_LABEL_NOT_FOUND, operand->label);
            }
            record = &table->records[labelPos];
            *binData += record->address;
            *binData <<= 2;
            if (record->isExternal) {
                *binData += (int)External;
                addNewExternToTable(externs, operand->label, cmdAddress);
            }
            else {
                *binData += (int)Relocatable;
            }
            break;
        case DYNAMIC:
            labelPos = isLabelExistsInTable(table, operand->label);
            if (labelPos == LABEL_NOT_EXISTS) {
                return errMessage(ERR_LABEL_NOT_FOUND, operand->label);
            }
            record = &table->records[labelPos];
            if (record->isExternal) {
                return errMessage(ERR_DYNAM_ADDRESS_EXTERN, operand->label);
            }
            num = getBitRangeFromInt(record->byteCodeForDynamic, operand->minNum, operand->maxNum);
            num = convertCompliment2(num, OPERAND_BYTE_SIZE);
            *binData += num;
            *binData <<= 2;
            *binData += (int)Absolute;
    }
    return NULL;
}

/**
 * update data tablee to start from ic + 1 after finishing building command array
 * @param table - SymbolsTable
 * @param ic - size of command array
 */
void updateSymbolsTableDataAddresses(SymbolsTable *table, int ic) {
    int i;
    for (i = 0; i < table->recordSize; ++i) { /* for each .data/.string add ic to its address */
        if (table->records[i].isCommand == false && table->records[i].isExternal == false) { /* update only .data\.string types */
            table->records[i].address += ic;
        }
    }
}

/**
 * gets a line and returns number of objects in memory to save for it
 * @param line - FileLine to check
 * @return amount of lines needed to be saved
 */
int getCommandSize(FileLine* line) {
    if (line->numOfCommandOprands == 0) {
        return 1; /* One command byte */
    }
    else if (line->numOfCommandOprands == 1) {
        return 2; /* One command byte + operand byte*/
    }
    else if (line->numOfCommandOprands == 2) {
        if (line->firstOperValue->addressingType == REGISTER &&
            line->secondOperValue->addressingType == REGISTER) {
            return 2; /* One command byte + one shared operand byte*/
        }
        else {
            return 3; /* One command byte + two operand bytes*/
        }
    }
    else {
        return 0;
        /* BUG should not get here */
    }
}

/* Public Methods */

/**
 * First compiler transition mainly for searching all labels and data array building
 * Algorithem:
 * 1. int ic = 0, dc = 0;
 * 2. Read next line
 * 3. check is label exists in the line start
 * 4. Rise isLabelExists flag - if it exists
 * 5. if it is .data or .string command? if not go to step 8
 * 6. if there is a label add it to the symbols table with the value of dc counter (if it exists in the table throw error)
 * 7. identify the data type and store it in the data AssemblyBytes
 * 7.1. update the dc counter according to the data size return to step 2(!)
 * 8. if the command is .extern or .entry (if not go to step 11)
 * 9. it is .extern order add it to the symbols table with extern flag on and no value
 * 10. return to step 2
 * 11. if there is a label add it to the symbols table with the value of ic counter (if it exists in the table throw error)
 * 12. Check command & operands type and calculate the command memory size
 * 13. Add to ic the value of ic + the calculated command size value
 * 14. go back to step 2
 * 15. When all lines have been process check for failures and see that the RAM size doesnt exceeds the maximum allowed
 * 16. Update the data labels (.data or .string) in the symbols table with the mathcing address (the dc location + calulated final ic)
 * @param fileContent parsed file lines
 * @param assembly final struct to fill with symbols and bytes, should be PARCIALLY FULL by the end of the method
 * @return Status (Pass/Fail)
 */
Status runFirstTransition(FileContent *fileContent, AssemblyStructure *assembly) {
    int i, calcCommandSize;
    Status status = Pass;
    assembly->ic = ASSEMBLY_CODE_START_ADDRESS;
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
                calcDataSize = line.firstOperValue->dataSize; /* check how much space to save in data array */
                firstByte = line.firstOperValue->data[0];
                isMemAllocOk = pushBytesFromIntArray(assembly->dataArray, line.firstOperValue->data,  /* push data into data array */
                                                     line.firstOperValue->dataSize);
            }
            else {
                calcDataSize = (int)strlen(line.firstOperValue->string) + 1; /* calculate how much space needed to save for string - +1 for /0 at the end of string */
                firstByte = line.firstOperValue->string[0];
                isMemAllocOk = pushBytesFromString(assembly->dataArray, line.firstOperValue->string);  /* Push string into data array */
            }

            if(isMemAllocOk == false) { /* if any error in memory allocation, throw error */
                printInternalError(ERR_DATA_RAM_OVERFLOW, fileContent->filename);
                status =  Fail;
                continue;
            }
            if (isLabelExists) { /* if .data/.string has label then add it to symbols table and check that it does not already exist */
                if (addNewLabelToTable(assembly->symbolsTable, line.label, assembly->dc, false, false, false, firstByte) == false) {
                    printCompileError(errMessage(ERR_LABEL_DEFINED_TWICE, line.label), fileContent->filename, line.lineNumber); /* throw error if label exist twice */
                    status =  Fail;
                    continue;
                }
            }
            assembly->dc += calcDataSize; /* increase dc size */
        }
            /* Handle External Symbols */
        else if (line.actionType == EXTERN || line.actionType == ENTRY) {
            if (isLabelExists) { /* Throw warning if .extern/.entry has label */
                printCompileWarning(errMessage(WARN_LABEL_IN_BAD_LOCATION, line.label), fileContent->filename, line.lineNumber);
            }
            /* Steps 9, 9.1, 10 */
            if (line.actionType == EXTERN) { /* add extern label to symbols table */
                addNewLabelToTable(assembly->symbolsTable, line.firstOperValue->entryOrExtern, 0, true, false, false, 0);
            }
        }
        else {/*  handle command line */

            /* Handle Command labels */
            if (isLabelExists) {  /* if there is label, add it to symbols table and calculate its binary bits (if needed by another dynamic cmd) */
                /* Step 11 */
                int binCommandForDynamicAddressing = buildBinaryCommand(line); /* calculate dynamic binary bits */
                if (addNewLabelToTable(assembly->symbolsTable, line.label, assembly->ic, false, true, false, /* add to sybolsTable and check no duplication */
                                       binCommandForDynamicAddressing) == false) {
                    printCompileError(errMessage(ERR_LABEL_DEFINED_TWICE, line.label), fileContent->filename, line.lineNumber); /* Throw error if label is duplicated */
                    status =  Fail;
                    continue;
                }
            }

            /* Steps 12, 13, 13.1, 14 */
            /* handle Command size */
            calcCommandSize = getCommandSize(&line);
            /* Step 14 */
            assembly->ic += calcCommandSize;
        }
    }
    if (status == Fail) {
        return Fail;
    }

    if (assembly->ic + assembly->dc >= MAX_CPU_MEMORY) { /* If more than 1000 commands, throw error */
        printInternalError(ERR_RAM_OVERFLOW, fileContent->filename);
        return Fail;
    }

    /* update the .data & .string records in the symbols table */
    updateSymbolsTableDataAddresses(assembly->symbolsTable, assembly->ic);
    return Pass;
}

/**
 * Second compiler transition is building the code array and locating the extrens/entries usages
 * Algorithm
 * 1. reset ic = 0
 * 2. read next line
 * 3. ignore label on the beginning of line
 * 4. is it .data/.string go to step 2
 * 5. is it .extern or .entry (if not go to step 7)
 * 6. if it's .entry order mark is as entry in the symbols table and return to step 2
 * 7. calculate the command code byte and the oprand/s byte/s
 * 8. store the command & operand/s next bytes in the code AssemblyBytes
 * 8.1 in case of 2 operands with REGISTER addressing they share the same byte in the code AssemblyBytes
 * 9. in crease the ic by the calulated amount
 * 10. return to step 2
 *
 * In case of a failure in the second transitions stop the file processing.
 *
 * @param fileContent
 * @param assembly final struct to fill with symbols and bytes, should be FULL by the end of the method
 * @return Status (Pass/Fail)
 */
Status runSecondTransition(FileContent *fileContent, AssemblyStructure *assembly) {
    int i;
    FileLine line;
    assembly->ic = ASSEMBLY_CODE_START_ADDRESS;

    for (i=0; i < fileContent->size; i++) { /* For every line in file */
        line = fileContent->line[i];

        if(DEBUG) {
            printSymbolTable(assembly->symbolsTable);
            printSymbolTable(assembly->externs);
            printAssemblyByte(assembly->codeArray);
            printAssemblyByte(assembly->dataArray);
        }

        /* Step: 4 */
        if (line.actionType == DATA || line.actionType == STRING) {
            /* do nothing */
            continue;
        }

        /* Step: 5 */
        if (line.actionType == EXTERN || line.actionType == ENTRY) {
            if (line.actionType == ENTRY) {
                if (setLabelIsEntryInTable(assembly->symbolsTable, line.firstOperValue->entryOrExtern) == false) {
                    printCompileError(errMessage(ERR_LABEL_NOT_DEFINED, line.firstOperValue->entryOrExtern), fileContent->filename, line.lineNumber);
                    return Fail;
                }
            }
        }
        /* handle command */
        else {
            /*  101 - num of command operands (2b) - command opcode (4b) - src addressing type (2b) - dest addressing type (2b) - E,R,A (2b) */
            char* errStr;
            int binCmd = buildBinaryCommand(line);
            bool isMemAllocOk = pushByteFromInt(assembly->codeArray, binCmd); /* add cmd to code array */
            assembly->ic++;
            /*  0000000000000-00 (data 13b - E,R,A (2b)) */
            /* Building Code Array */
            if (line.numOfCommandOprands == 1){ /* if one operand */
                int binData = 0;
                errStr = buildBinaryData(&binData, line.firstOperValue, assembly->symbolsTable, assembly->externs, true, assembly->ic);
                if (errStr != NULL) { /* if there was an issue in building binary data of first operand, throw error */
                    printCompileError(errStr, fileContent->filename, line.lineNumber);
                    return Fail;
                }
                isMemAllocOk &= pushByteFromInt(assembly->codeArray, binData); /* add first operand byte array to code array */
                assembly->ic++;
            }
            else if (line.numOfCommandOprands == 2) { /* if 2 operands */
                int firstBinData = 0, secondBinData = 0;
                errStr = buildBinaryData(&firstBinData, line.firstOperValue, assembly->symbolsTable, assembly->externs, false, assembly->ic); /* calculate first operand byte array */
                if (errStr != NULL) { /* if there was an issue in building binary data of first operand, throw error */
                    printCompileError(errStr, fileContent->filename, line.lineNumber);
                    return Fail;
                }
                errStr = buildBinaryData(&secondBinData, line.secondOperValue, assembly->symbolsTable, assembly->externs, true, assembly->ic+1); /* calculate second operand byte array */
                if (errStr != NULL) { /* if there was an issue in building binary data of first operand, throw error */
                    printCompileError(errStr, fileContent->filename, line.lineNumber);
                    return Fail;
                }
                if (line.firstOperValue->addressingType == REGISTER && line.secondOperValue->addressingType == REGISTER){ /* if both operands are registers, they share a byte array */
                    isMemAllocOk &= pushByteFromInt(assembly->codeArray, firstBinData + secondBinData);
                    assembly->ic++;
                }
                else{
                    isMemAllocOk &= pushByteFromInt(assembly->codeArray, firstBinData);
                    isMemAllocOk &= pushByteFromInt(assembly->codeArray, secondBinData);
                    assembly->ic += 2;
                }
            }

            if(isMemAllocOk == false){ /* if there is an issue with memory allocation, throw error */
                printInternalError(ERR_CODE_RAM_OVERFLOW, fileContent->filename);
                return Fail;
            }
        }

    }
    return Pass; /* Done and ready to save assembly files! */
}