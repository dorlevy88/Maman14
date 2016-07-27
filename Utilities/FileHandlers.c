//
// Created by Dor Levy on 7/26/16.
//

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "FileHandlers.h"

//Creates the following array structure - [Label, Command, To, From], NULL if not exist
FileLine parseLine(char* rawLine) {
    FileLine parsedLine;

    // Get first string
    char* string = strtok(rawLine, " ,\n");

    for (int i=0; i < NUM_OF_LINE_ELEMENTS; i++) {
        if (string != NULL) {
            if (i==0) { //Handle label position
                if (strchr(string, ':') != NULL) {
                    //found Label
                    parsedLine.word[i] = string;
                }
                else { //No Label
                    parsedLine.word[i] = NULL;
                }
            } //Handle Rest of the cases (1,2,3)
            else {
                parsedLine.word[i] = string;
            }
        }
        else { // Fills Rest of array with NULLs
            parsedLine.word[i]= NULL;
        }

        string = strtok(NULL, " ,\n");
    }

    return parsedLine;
}

FileContent getFileContent(char* filename) {
    FILE * fr;
    char line[MAX_LINE_SIZE];
    FileContent fileContent;
    int lineCounter = 0;

    fr = fopen (filename, "r"); // Open the file for reading
    if (fr == NULL)
    {
        printf("File %s does not exist", filename);
    }
    while(fgets(line, sizeof(line), fr) != NULL)   //get a word.  done if NULL
    {
        char *lineCopy;
        lineCopy = (char *) malloc(sizeof(line));
        strcpy(lineCopy, line);
        FileLine parsedLine = parseLine(lineCopy);
        fileContent.line[lineCounter] = parsedLine;
        lineCounter++;
    }
    fileContent.numRows = lineCounter;
    fclose(fr);  /* close the file prior to exiting the routine */

    return fileContent;
}