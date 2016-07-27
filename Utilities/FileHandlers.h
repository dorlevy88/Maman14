//
// Created by Dor Levy on 7/26/16.
//

#ifndef MAMAN14_FILEHANDLERS_H
#define MAMAN14_FILEHANDLERS_H

#endif //MAMAN14_FILEHANDLERS_H


#define NUM_OF_LINE_ELEMENTS 4
#define MAX_FILE_LINES 1000
#define MAX_LINE_SIZE 100

typedef struct FileLine {

    char* word[NUM_OF_LINE_ELEMENTS];

} FileLine;


typedef struct FileContent {

    FileLine line[MAX_FILE_LINES];
    int numRows;

} FileContent;

FileContent getFileContent(char* filename);