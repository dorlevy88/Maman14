//
// Created by Or Zamir on 7/16/16.
//
#include "FirstTransition.h"
#include "../Utilities/Logger.h"
#include "CompilerErrors.h"

#define MAX_CPU_MEMORY 1000

void UpdateSymbolsTableDataAddresses(SymbolsTable* table, int ic) {
    for (int i = 0; i < table->recordSize; ++i) {
        if (table->records[i].isCommand == false && table->records[i].isExternal == false) { //update only .data\.string types
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
            sizeInIc++; //command byte
            if (line->firstOperValue->addressingType == REGISTER &&
                line->secondOperValue->addressingType == REGISTER) {
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

bool RunFirstTransition(FileContent* fileContent, AssemblyStructure* assembly) {
    //1. int ic = 0, dc = 0;
    //2. Read line
    //3. check is symbol exists in the first field
    //4. Rise isLabelExists flag - if it exists
    //5. if it is data storage order? (.data or .string) if not go to step 8
        //6. if there is a symbol add it to the symbols table with the value of dc counter (if it exists in the table throw error)
        //7. identify the data type and store it in memory
        //7.1. update the dc counter according to the data size return to step 2(!)
    //8. if the order is .extern or .entry (if not go to step 11)
        //9. is it .extern order
        //9.1 add it to the EXTERN symbols table with no value
        //10. return to step 2
    //11. if there is a label add it to the symbols table with the value of ic counter (if it exists in the table throw error)
    //12. search the commands table if not exists throw unknown command code error
    //13. Check command operand type and calculate the command memory size (L)
    //13.1 You can produce the order code here
    //14. Add to ic the value of ic + the calculated L value (ic += L)
    //15. go back to step 2

    assembly->ic = assembly->startAddress;
    assembly->dc = 0;

    for (int i=0; i < fileContent->size; i++){ //For every line in file
        FileLine line = fileContent->line[i];


        bool isLabelExists = false;

        if (line.label != NULL) { //If Label Exists for line
            isLabelExists = true;
        }

        //Handle Data Storage Symbols
        if (line.actionType == DATA || line.actionType == STRING) {
            //Steps 6 ,7 and 7.1

            //Push into data array
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

            if (AddNewLabelToTable(assembly->symbolsTable, line.label, assembly->dc, false, false, false, firstByte) == false) {
                PrintCompileError(ERR_LABEL_DEFINED_TWICE, line.label, line.lineNumber);
                return false;
            }
            assembly->dc += calcDataSize;
        }
        //Handle External Symbols
        else if (line.actionType == EXTERN || line.actionType == ENTRY) {
            if (isLabelExists) {
                PrintCompileWarning(WARN_LABEL_IN_BAD_LOCATION, line.label, line.lineNumber);
            }
            //Steps 9, 9.1, 10
            if (line.actionType == EXTERN) {
                AddNewLabelToTable(assembly->symbolsTable, line.firstOperValue->entryOrExtern, 0, true, false, false, 0);
                //TODO: Should we send error if label exists in table?
            }
        }
        else {// handle command line

            //Handle Command labels
            if (isLabelExists) {
                //Step 11
                int binCommandForDynamicAddressing = buildBinaryCommand(line);
                if (AddNewLabelToTable(assembly->symbolsTable, line.label, assembly->ic, false, true, false,
                                       binCommandForDynamicAddressing) == false) {
                    PrintCompileError(ERR_LABEL_DEFINED_TWICE, line.label, line.lineNumber);
                    return false;
                }
            }

            //Steps 12, 13, 13.1, 14
            //handle Command size
            int calcCommandSize = getCommandSize(&line);
            //Step 14
            assembly->ic += calcCommandSize;
        }
    }

    if (assembly->ic + assembly->dc >= MAX_CPU_MEMORY) {
        //TODO: Throw error program is larger than CPU memory
    }

    UpdateSymbolsTableDataAddresses(assembly->symbolsTable, assembly->ic);
    return true;
}