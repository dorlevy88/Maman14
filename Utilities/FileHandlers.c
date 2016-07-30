//
// Created by Dor Levy on 7/26/16.
//

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <inttypes.h>
#include "FileHandlers.h"

#define MIN_NUM_SIZE -16384
#define MAX_NUM_SIZE 16384
#define INVALID_NUM_TOKEN -999999

bool isLabelValid(char* label){
    //TODO: check if starts with char
    //Check if only numbers and chars
    //Check up to 30 chars (not including ':'
}


int getInFromString(char *string){
    if (strlen(string) == 0){ //Check string is not empty
        return INVALID_NUM_TOKEN;
    }
    char* ptr;
    intmax_t num = strtoimax(string, &ptr, 10);
    if (num < MIN_NUM_SIZE || num > MAX_NUM_SIZE) {
        //TODO: num bigger than 15 bits
        return INVALID_NUM_TOKEN;
    }
    if (strlen(ptr) != 0) {
        //TODO: not a number
        return INVALID_NUM_TOKEN;
    }
    return (int)num;
}

Operand getOperand(char* operand){
    Operand oper;
    if (operand[0] == '#'){ //check direct addressing
        int num = getInFromString(++operand);
        if (num != INVALID_NUM_TOKEN) { //Miyadi
            //string is number
            oper.addressingType = NUMBER;
            oper.value = num;
            return oper;
        }
        else {
            //TODO: not a number syntax error
        }
    }
    else if (operand[0] == 'r' && strlen(operand) == 2 && operand[1] >= '0' && operand[1] <= '7') { //Oger
        oper.addressingType = REGISTER;
        
        return oper;
    }
    else if (operand[0] != '[' && operand[0] != ']' &&
            strchr(operand, '[') != NULL && strchr(operand, ']') != NULL) { //Dinami Yashir
        oper.addressingType = DYNAMIC;
        return oper;
    }
    else if (isLabelValid(operand)) { //Yeshir
        oper.addressingType = DIRECT;
        return oper;
    }
    else {
        //TODO: Syntax error
    }
}

void checkTwoOperands(char* rawOperandsString, FileLine parsedLine){
    char* firstOperand = strtok(rawOperandsString, " ,"); //get first operand - split by comma and/or space
    char* secondOperand = strtok(NULL, " ,"); //get second operand - split by comma and/or space
    if (strtok(NULL, " ,") != NULL){
        //TODO: stderr for syntax error - too many operands
        parsedLine.hasSyntaxError = true;
        return;
    }
}

void checkOneOperand(){
    //TODO:
}

void checkNoOperand(){
    //TODO:
}

void checkDataOperand() {
    //TODO:
}

void checkStringOperand() {
    //TODO:
}

void validateActionAndOperands(char* rawOperandsString, FileLine parsedLine) {
    if (strcmp(parsedLine.action, "mov") == 0) {
        parsedLine.actionType =;
        checkTwoOperands(rawOperandsString, parsedLine);
        //TODO: check operands types validity
    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }
    else if (strcmp(parsedLine.action, "mov") == 0){

    }




    if (parsedLine.hasSyntaxError){
        return;
    }


    const char* ALLOWED_ACTION_NAMES[] = {"mov", "cmp"};
    for (int i = 0; i < sizeof(ALLOWED_ACTION_NAMES); ++i) {
        if (strcmp(ALLOWED_ACTION_NAMES[i], action) == 0) {
            
        }
    }
}

FileLine lineValidator(char* rawLine, int lineCounter) {
    FileLine parsedLine;
    parsedLine.lineNum = lineCounter;

    char* string = strtok(rawLine, " "); //Get first string

    //Handle empty lines
    if (string == NULL){
        parsedLine.isEmptyOrComment = true;
        return parsedLine;
    }//Handle Comment Lines
    else if (strlen(string) >= 1 && string[0] == ';'){
        parsedLine.isEmptyOrComment = true;
        return parsedLine;
    }

    //Handle labels
    size_t strSize = strlen(string);
    if (string[strSize - 1] == ':') { //Label Found
        isLabelValid(string);
        
        strcpy(parsedLine.label, string);      //get the label
        char* action = strtok(NULL, " "); //Get action string

    }
    strcpy(parsedLine.action, string);      //get the action

    char* rawOperandsString = strstr(rawLine, parsedLine.action) + strlen(parsedLine.action);

    validateActionAndOperands(rawOperandsString, parsedLine);




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

