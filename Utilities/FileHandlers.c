//
// Created by Dor Levy on 7/26/16.
//

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "FileHandlers.h"

//Creates the following array structure - [Label, Command, To, From], NULL if not exist
//FileLine parseLine(char* rawLine) {
//    FileLine parsedLine;
//
//    // Get first string
//    char* string = strtok(rawLine, " ,\n");
//
//    for (int i=0; i < NUM_OF_LINE_ELEMENTS; i++) {
//        if (string != NULL) {
//            if (i==0) { //Handle label position
//                if (strchr(string, ':') != NULL) {
//                    //found Label
//                    parsedLine.word[i] = string;
//                }
//                else { //No Label
//                    parsedLine.word[i] = NULL;
//                }
//            } //Handle Rest of the cases (1,2,3)
//            else {
//                parsedLine.word[i] = string;
//            }
//        }
//        else { // Fills Rest of array with NULLs
//            parsedLine.word[i]= NULL;
//        }
//
//        string = strtok(NULL, " ,\n");
//    }
//
//    return parsedLine;
//}

//#define ALLOWED_ACTION_NAMES = ["mov", "cmp"]

ActionTypes checkActionName(char* action) {
    const char* ALLOWED_ACTION_NAMES[] = {"mov", "cmp"};
    for (int i = 0; i < sizeof(ALLOWED_ACTION_NAMES); ++i) {
        if (strcmp(ALLOWED_ACTION_NAMES[i], action) == 0) {
            
        }
    }
}

FileLine lineValidator(char* rawLine, int lineCounter) {
    FileLine parsedLine;

    char* string = strtok(rawLine, " "); //Get first string

    //Handle empty lines or comment line
    if (string == NULL){
        parsedLine.isEmptyOrComment = true;
        return parsedLine;
    }
    else if (strlen(string) >= 1 && string[0] == ';'){
        parsedLine.isEmptyOrComment = true;
        return parsedLine;
    }
    parsedLine.lineNum = lineCounter;

    //Handle labels
    if (string[strlen(string) - 1] == ':') {
        //TODO: check label size
        //more checks?
        
        parsedLine.label = string;      //get the label
        string = strtok(NULL, " ");  //get next string
    }
    
    //Handle actions
    for


    return parsedLine;
}

FileContent getFileContent(char* filename) {
    FILE * fr;
    char line[MAX_LINE_SIZE];
    FileContent fileContent;
    int lineCounter = 1;
    int arrayIndex = 0;

    fr = fopen (filename, "r"); // Open the file for reading
    if (fr == NULL)
    {
        //TODO: send an error to the stderr and continue to the next file
        printf("File %s does not exist", filename);
        fileContent.hasError = true;
        return fileContent;
    }
    while(fgets(line, sizeof(line), fr) != NULL)   //get a word.  done if NULL
    {
        char *lineCopy;
        lineCopy = (char *) malloc(sizeof(line));
        strcpy(lineCopy, line);
        FileLine parsedLine = lineValidator(lineCopy, lineCounter++);
        if (parsedLine.hasSyntaxError) {
            fileContent.hasError = true;
            return fileContent;
        }
        if (parsedLine.isEmptyOrComment == false) {
            fileContent.line[arrayIndex++] = parsedLine;

        }
    }
    fileContent.size = arrayIndex;
    fclose(fr);  /* close the file prior to exiting the routine */

    return fileContent;
}

