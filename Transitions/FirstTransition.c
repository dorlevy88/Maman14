//
// Created by Or Zamir on 7/16/16.
//


#include <stdbool.h>
#include <memory.h>
#include "../DataStructures/AssemblyStructure.h"
#include "FirstTransition.h"
#include "../DataStructures/SymbolsTable.h"


void RunFirstTransition(FileContent fileContent, AssemblyStructure assembly) {
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
    //11. if there is a symbol add it to the symbols table with the value of ic counter (if it exists in the table throw error)
    //12. search the commands table if not exists throw unknown command code error
    //13. Check command operand type and calculate the command memory size (L)
    //13.1 You can produce the order code here
    //14. Add to ic the value of ic + the calculated L value (ic += L)
    //15. go back to step 2

    assembly.ic = 0;
    assembly.dc = 0;

    for (int i=0; i < fileContent.size; i++){ //For every line in file
        FileLine line = fileContent.line[i];
        bool isLabelExists = false;

        if (line.label != NULL) { //If Label Exists for line
            isLabelExists = true;
        }

        if (line.actionType == DATA || line.actionType == STRING) {  //Handle Data Storage Symbols
            //TODO:Steps 6 ,7 and 7.1
            int labelPos = isLabelExistsInTable(assembly.symbolsTable, line.label);
            if(labelPos != -1) {
                //TODO: error label exists in table
            }
            SymbolRecord rec = {
                    label: line.label,
                    address: assembly.dc,
                    isExternal: false,
                    isCommand: false
            };
            assembly.symbolsTable.record[assembly.symbolsTable.size++] = rec;

            int calcDataSize = 0;
            if (line.actionType == DATA ) {
                calcDataSize = sizeof(line.firstOperValue.data);
                //TODO: add data to assembly.dataArray
            }
            else {
                calcDataSize = sizeof(line.firstOperValue.string);
                //TODO: add string to assembly.dataArray
            }
            assembly.dc += calcDataSize;
            continue;
        }
        else if (line.actionType == EXTERN || line.actionType == ENTRY) { //Handle External Symbols
            //TODO:Steps 9, 9.1, 10
            continue;
        }
        else if (isLabelExists){ //Handle Code Storage Symbols
            //TODO: Step 11
        }
        //TODO: Handle command - Step 12
        //TODO: Handle Command Addressing Types - Step 13
        //TODO: Handle Order Code - Step 13.1
        //TODO: Handle IC and DC Counters - Step 14
    }


}