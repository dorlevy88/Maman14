////
//// Created by Or Zamir on 7/16/16.
////
//
//#include <stdio.h>
//#include "../DataStructures/AssemblyStructure.h"
//#include "../DataStructures/Command.h"
//#include "SecondTransition.h"
//
//
//void RunSecondTransition(FileContent fileContent, AssemblyStructure assembly) {
//
//    //1. ic = 0
//    //2. read word if done go to step 11
//    //3. ignore label on the beginning of line
//    //4. is it .data/.string go to step 2 else continue
//    //5. is it .extern or .entry (if not go to step 7)
//        //6. identify the order
//        //6.1. act accordingly
//        //6.2 if it's .entry order add the entry symbols and return to step 2
//    //7. estimate the operands look at the command table and switch to the correct command code
//    //8. store the operands in the next byte
//    //8.1 if it a symbol find a match in the symbols table
//    //8.2 calculate addressing & their matching codes
//    //9. ic += L
//    //10. return to step 2
//    //11. save to a different file:
//    //11.1 size of the program
//    //11.2 size of the data
//    //11.3 extern symbols table
//    //11.4 symbols table with entry points marks
//
//    int tmpIc = assembly.ic;
//    assembly.ic = 100;
//
//    for (int i=0; i < fileContent.size; i++) { //For every line in file
//        FileLine line = fileContent.line[i];
//
//        //Step: 4
//        if (line.actionType == DATA || line.actionType == STRING) {
//            //do nothing
//        }
//        //Step: 5
//        else if (line.actionType == EXTERN || line.actionType == ENTRY) {
//            if (line.actionType == ENTRY) {
//                if (SetLabelAddressInTable(assembly.symbolsTable, line.firstOperValue.entryOrExtern, assembly.ic,
//                                           DYNAMIC_ADDRESSING_NOT_AVAILABLE) == false) {
//                    //TODO: error label does not exists
//                }
//            }
//        }
//        //handle command
//        else{
//            // 101 - num od command operands (2b) - command opcode (4b) - src addressing type (2b) - dest addressing type (2b) - E,R,A (2b)
//            int binCmd = buildBinaryCommand(line);
//            bool isMemAllocOk = PushByteFromInt(assembly.codeArray, binCmd);
//
//            assembly.ic++;
//            // 0000000000000-00 (data 13b - E,R,A (2b))
//            int binData = 0;
//            if (line.numOfCommandOprands >= 1) {
//                binData = buildBinaryData(line.firstOperValue, assembly.symbolsTable);
//                if (line.numOfCommandOprands == 2) {
//                    if (line.secondOperValue.addressingType == REGISTER){
//                        int firstBinData = binData;
//                        binData = buildBinaryData(line.secondOperValue, assembly.symbolsTable);
//                        firstBinData <<= 6;
//                        firstBinData |= binData;
//                        isMemAllocOk &= PushByteFromInt(assembly.codeArray, firstBinData);
//                        assembly.ic++;
//                    }
//                    else {
//                        isMemAllocOk |= PushByteFromInt(assembly.codeArray, binData);
//                        binData = buildBinaryData(line.secondOperValue, assembly.symbolsTable);
//                        isMemAllocOk &= PushByteFromInt(assembly.codeArray, binData);
//                        assembly.ic += 2;
//                    }
//                }
//            }
//
//            if(isMemAllocOk == false){
//                //TODO: Throw an error size of array exceeded
//            }
//        }
//
//    }
//
//    if (assembly.ic != tmpIc) {
//        //BUG: something went wrong with the algorithm
//    }
//
//    //Done and ready to save!
//}