//
// Created by Or Zamir on 7/16/16.
//

#include "SecondTransition.h"



void RunSecondTransition() {

    //1. ic = 0
    //2. read line if done go to step 11
    //3. if the first field is a symbol skip it
    //4. is it imaginary order go to step 2 else continue
    //5. is it .extern or .entry (if not go to step 7)
    //6. identify the order
        //6.1. act accordingly
        //6.2 if it's .entry order add the entry symbols and return to step 2
    //7. estimate the operands look at the command table and switch to the correct command code
    //8. store the operands in the next byte
    //8.1 if it a symbol find a match in the symbols table
    //8.2 calculate addressing & their matching codes
    //9. ic += L
    //10. return to step 2
    //11. save to a different file:
    //11.1 size of the program
    //11.2 size of the data
    //11.3 extern symbols table
    //11.4 symbols table with entry points marks
}