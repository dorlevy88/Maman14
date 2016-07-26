//
// Created by Dor Levy on 7/10/16.
//

#include "FileReader.h"
#include <stdio.h>
#include <string.h>

const int MAX_FILE_LINES = 1000;
const int MAX_LINE_SIZE =  100;
const int NUM_OF_LINE_ELEMENTS = 4;


//Creates the following array structure - [Label, Command, To, From], NULL if not exist
char** parseLine(char* rawLine) {
    char* parsedLine[NUM_OF_LINE_ELEMENTS];

    // Get first string
    char* string = strtok(rawLine, " ,");

    for (int i=0; i < NUM_OF_LINE_ELEMENTS; i++) {
        if (string != NULL) {
            if (i==0) { //Handle label position
                if (strchr(string, ':') != NULL) {
                    //found Label
                    parsedLine[i] = string;
                }
                else { //No Label
                    parsedLine[i] = NULL;
                }
            } //Handle Rest of the cases (1,2,3)
            else {
                parsedLine[i] = string;
            }
        }
        else { // Fills Rest of array with NULLs
            parsedLine[i]= NULL;
        }

        string = strtok(NULL, " ,");
    }

    return parsedLine;
}

char*** getFileContent(char* filename) {
    FILE * fr;
    char line[MAX_LINE_SIZE];
    char** parsedFile[MAX_FILE_LINES];
    int lineCounter = 0;

    fr = fopen (filename, "r"); // Open the file for reading
    if (fr == NULL)
    {
        printf("File %s does not exist", filename);
    }
    while(fgets(line, sizeof(line), fr) != NULL)   //get a line.  done if NULL
    {
//TODO:        if (sizeof(parsedFile) >= MAX_FILE_LINES) {
//            printf("Not enough memory to hold your program. Exisiting!");
//        }
        printf("%s", line);
        char** parsedLine = parseLine(line);
        parsedFile[lineCounter] = parsedLine;
        lineCounter++;
    }
    fclose(fr);  /* close the file prior to exiting the routine */

}

char* readLine(FILE *filePointer) {

}



