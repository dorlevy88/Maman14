//
// Created by Or Zamir on 7/16/16.
//

#include <stdio.h>
#include "../DataStructures/AssemblyStructure.h"
#include "FirstTransition.h"


void RunFirstTransition(FILE* file, AssemblyStructure assemblyStructure) {
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
}