//
// Created by Dor Levy on 7/26/16.
//

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "Logger.h"
#include <inttypes.h>
#include <ctype.h>
#include "FileHandlers.h"

#define MAX_LABEL_SIZE 30
#define MIN_NUM_SIZE -16384
#define MAX_NUM_SIZE 16384
#define INVALID_NUM_TOKEN -999999
#define MAX_DYNAMIC_OPERAND 13 //max exact number

bool isLabelValid(char* label){
    if (strlen(label) > 0){ //check label is not empty
        if (isalpha(label[0])){ // Check first char is letter
            for (int i = 1; i < strlen(label); i++) {
                if ((!(isalpha(label[i])) && !(isdigit(label[i]))) ||  // All chars are digits or letters
                    i == MAX_LABEL_SIZE) {  //Size of Label is less than MAX_LABEL_SIZE (30)
                    return false;
                }
            }
            return true;
        }
        else{
            return false; //First Char in label is not a letter
        }
    }
    else{
        return false; //Label is 0 Chars
    }
}

int getIntFromString(char *string){
    if (strlen(string) == 0){ //Check string is not empty
        return INVALID_NUM_TOKEN;
    }
    char* ptr;
    intmax_t num = strtoimax(string, &ptr, 10); //cast string to number
    if (num < MIN_NUM_SIZE || num > MAX_NUM_SIZE) { //check number could be casted to 15 bits array
        return INVALID_NUM_TOKEN;
    }
    if (strlen(ptr) != 0) { //check if there was any other chars except for digits in the number
        return INVALID_NUM_TOKEN;
    }
    return (int)num;
}

Operand getOperand(char* operand) {
    Operand oper = { .hasError = false, .registerNum = -1 };

    //check direct addressing
    if (operand[0] == '#') {
        int num = getIntFromString(++operand);
        if (num != INVALID_NUM_TOKEN) { //Miyadi
            //string is number
            oper.addressingType = NUMBER;
            oper.value = num;
            return oper;
        }
        else {
            oper.err = strcat("expected a number, but received ", operand);
            oper.hasError = true;
            return oper;
        }
    }
    // Oger
    if (operand[0] == 'r' && strlen(operand) == 2) {
        if (operand[1] >= '0' && operand[1] <= '7') {
            oper.addressingType = REGISTER;
            oper.registerNum = (int)operand[1] - '0';
            return oper;
        }
        else {
            oper.err = strcat("Expected register number between 0 and 7, received - ", operand[1]);
            oper.hasError = true;
        }
    }
    //Dinami Yashir
    char *openBracketLocationString = strchr(operand, '[');
    char *closeBracketLocationString = strchr(operand, ']');
    if (openBracketLocationString != NULL && closeBracketLocationString != NULL) { //Check we should be in Dinami Yashir
        oper.addressingType = DYNAMIC;
        if (closeBracketLocationString > openBracketLocationString && operand[0] != '[' &&
            strchr(operand, '[') - operand <= MAX_LABEL_SIZE &&
            operand[strlen(operand) - 1] == ']') { //Check '[' before ']', '[' not in first char, ']' is at the end, Label size is less then max size
            char* label = (char*)malloc(strchr(operand, '[') - operand);
            memcpy(label, operand, strchr(operand, '[') - operand); // copy label - everything before '['
            if (!isLabelValid(label)) {
                oper.hasError = true;
                oper.err = strcat("Label is not valid - ", label);
            }
            oper.label = label;
            // get min number
            // char minNum[MAX_DYNAMIC_OPERAND_SIZE];
            size_t minDashLocation = strchr(openBracketLocationString, '-') - openBracketLocationString; //get the char num where - exists
            char* minNum = (char*)malloc(minDashLocation - 1);
            strncpy(minNum, openBracketLocationString + 1, minDashLocation - 1);
            int numToCheck = getIntFromString(minNum);
            if (numToCheck != INVALID_NUM_TOKEN ||
                numToCheck > MAX_DYNAMIC_OPERAND) { //Can't be more than the num 13
                oper.minNum = numToCheck;
            }
            else {
                oper.err = "Operand num is out of bound";
                oper.hasError = true;
                return oper;
            }
            // get max number
            char *maxDashLocationString = strchr(openBracketLocationString, '-');
            char* maxNum = (char*)malloc(strlen(maxDashLocationString) - 2);
            strncpy(maxNum, maxDashLocationString + 1, strlen(maxDashLocationString) - 2);
            int numToCheckMax = getIntFromString(maxNum);
            if (numToCheckMax != INVALID_NUM_TOKEN ||
                numToCheckMax > MAX_DYNAMIC_OPERAND) { //Can't be more than the num 13
                oper.maxNum = numToCheckMax;
                return oper;
            }
            else {
                oper.err = "Operand num is out of bound";
                oper.hasError = true;
                return oper;
            }
        }
    }

    //Yashir
    if (isLabelValid(operand)) {
        oper.addressingType = DIRECT;
        oper.label = operand;
        return oper;
    }
    else {
        oper.err = "unknown error";
        oper.hasError = true;
        return oper;
    }
}

FileLine checkTwoOperands(char* rawOperandsString, FileLine parsedLine){
    char* firstRawOperand = strtok(rawOperandsString, " ,\t"); //get first operand - split by comma and/or space
    char* secondRawOperand = strtok(NULL, " ,\t"); //get second operand - split by comma and/or space
    if (secondRawOperand == NULL){
        PrintSyntaxError("Expected two operands, received one", parsedLine.lineNum);
        parsedLine.hasSyntaxError = true;
        return parsedLine;
    }
    if (strtok(NULL, " ,\t") != NULL){
        PrintSyntaxError("Expected two operands, received more", parsedLine.lineNum);
        parsedLine.hasSyntaxError = true;
        return parsedLine;
    }
    Operand firstOperand = getOperand(firstRawOperand);
    Operand secondOperand = getOperand(secondRawOperand);
    if (firstOperand.hasError){
        PrintSyntaxError(firstOperand.err, parsedLine.lineNum);
        parsedLine.hasSyntaxError = true;
        return parsedLine;
    }
    else if (secondOperand.hasError){
        PrintSyntaxError(secondOperand.err, parsedLine.lineNum);
        parsedLine.hasSyntaxError = true;
        return parsedLine;
    }
    parsedLine.numOfCommandOprands = 2;
    parsedLine.firstOperValue = firstOperand;
    parsedLine.firstOperand = firstRawOperand;
    parsedLine.secondOperValue = secondOperand;
    parsedLine.secondOperand = secondRawOperand;
    return parsedLine;
}

FileLine checkOneOperand(char* rawOperandsString, FileLine parsedLine){
    char* firstRawOperand = strtok(rawOperandsString, " ,\t"); //get first operand - split by comma and/or space
    if (strtok(NULL, " ,\t") != NULL){ //Operand 2 is not empty
        parsedLine.hasSyntaxError = true;
        PrintSyntaxError("Expected One operand - Received more", parsedLine.lineNum);
        return parsedLine;
    }
    else if (firstRawOperand == NULL){ //Operand 1 is empty
        parsedLine.hasSyntaxError = true;
        PrintSyntaxError("Expected One operand - Received None", parsedLine.lineNum);
        return parsedLine;
    }
    Operand firstOperand = getOperand(firstRawOperand);
    if (firstOperand.hasError){
        PrintSyntaxError(firstOperand.err, parsedLine.lineNum);
        parsedLine.hasSyntaxError = true;
        return parsedLine;
    }
    parsedLine.numOfCommandOprands = 1;
    parsedLine.firstOperand = firstRawOperand;
    parsedLine.firstOperValue = firstOperand;
    return parsedLine;
}

FileLine checkNoOperand(char* rawOperandsString, FileLine parsedLine){
    char* firstRawOperand = strtok(rawOperandsString, " ,\t");
    if (firstRawOperand != NULL){ //Operand 1 is not empty
        parsedLine.hasSyntaxError = true;
        PrintSyntaxError("Expected no operand - Received more", parsedLine.lineNum);
        return parsedLine;
    }
    parsedLine.numOfCommandOprands = 0;
    return parsedLine;
}

void checkDataOperand() {
    //TODO:
}

void checkStringOperand() {
    //TODO:
}
void checkExternOperand() {
    //TODO:
}

void checkEntryOperand() {
    //TODO:
}

FileLine validateActionAndOperands(char* rawOperandsString, FileLine parsedLine) {
    if (strcmp(parsedLine.action, "mov") == 0) {
        parsedLine.actionType = MOV;
        parsedLine = checkTwoOperands(rawOperandsString, parsedLine);
    }
    else if (strcmp(parsedLine.action, "cmp") == 0){
        parsedLine.actionType = CMP;
        parsedLine = checkTwoOperands(rawOperandsString, parsedLine);
    }
    else if (strcmp(parsedLine.action, "add") == 0){
        parsedLine.actionType = ADD;
        parsedLine = checkTwoOperands(rawOperandsString, parsedLine);
    }
    else if (strcmp(parsedLine.action, "sub") == 0){
        parsedLine.actionType = SUB;
        parsedLine = checkTwoOperands(rawOperandsString, parsedLine);
    }
    else if (strcmp(parsedLine.action, "lea") == 0){
        parsedLine.actionType = LEA;
        parsedLine = checkTwoOperands(rawOperandsString, parsedLine);
    }
    else if (strcmp(parsedLine.action, "clr") == 0){
        parsedLine.actionType = CLR;
        parsedLine = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(parsedLine.action, "not") == 0){
        parsedLine.actionType = NOT;
        parsedLine = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(parsedLine.action, "inc") == 0){
        parsedLine.actionType = INC;
        parsedLine = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(parsedLine.action, "dec") == 0){
        parsedLine.actionType = DEC;
        parsedLine = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(parsedLine.action, "jmp") == 0){
        parsedLine.actionType = JMP;
        parsedLine = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(parsedLine.action, "bne") == 0){
        parsedLine.actionType = BNE;
        parsedLine = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(parsedLine.action, "red") == 0){
        parsedLine.actionType = RED;
        parsedLine = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(parsedLine.action, "prn") == 0){
        parsedLine.actionType = PRN;
        parsedLine = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(parsedLine.action, "jsr") == 0){
        parsedLine.actionType = JSR;
        parsedLine = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(parsedLine.action, "rts") == 0){
        parsedLine.actionType = RTS;
        parsedLine = checkNoOperand(rawOperandsString ,parsedLine);
    }
    else if (strcmp(parsedLine.action, "stop") == 0){
        parsedLine.actionType = STOP;
        parsedLine = checkNoOperand(rawOperandsString ,parsedLine);
    }
    else if (strcmp(parsedLine.action, ".data") == 0){
        checkDataOperand();
    }
    else if (strcmp(parsedLine.action, ".string") == 0){
        checkStringOperand();
    }
    else if (strcmp(parsedLine.action, ".entry") == 0){
        checkEntryOperand();
    }
    else if (strcmp(parsedLine.action, ".extern") == 0){
        checkExternOperand();
    }
    else { //Command is not known
        parsedLine.hasSyntaxError = true;
        return parsedLine;
    }
    return parsedLine;
}

FileLine lineValidator(char* rawLineIn, int lineCounter) {
    char* rawLine = (char*)malloc(strlen(rawLineIn));
    strcpy(rawLine, rawLineIn);
    FileLine parsedLine = { .lineNum = lineCounter , .actionType = UNKNOWN_ACTION, .lineType = UNKNOWN_LINE_TYPE };

    char* string = strtok(rawLine, " \t\n"); //Get first string

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
        if (strSize > MAX_LABEL_SIZE + 1){
            char* errMsg = strcat("label size is more than allowed - ", MAX_LABEL_SIZE);
            PrintSyntaxError(errMsg ,parsedLine.lineNum);
            parsedLine.hasSyntaxError = true;
        }
        char* parsedLabel = (char*) malloc(strSize - 1);
        memcpy(parsedLabel, string, strSize - 1);  //get the label

        if (isLabelValid(parsedLabel)){
            parsedLine.label = parsedLabel;
        }
        else {
            parsedLine.hasSyntaxError = true;
        }

        string = strtok(NULL, " \t\n"); //Get action string

    }
    parsedLine.action = string;      //get the action

    char* rawOperandsString = strtok(NULL, "\n"); // get remaining of line

    parsedLine = validateActionAndOperands(rawOperandsString, parsedLine);

    return parsedLine;
}

FileContent getFileContent(char* filename) {
    FILE * fr;
    char line[MAX_LINE_SIZE];
    FileContent fileContent = { .size = 0, .hasError = false };
    int lineCounter = 1;
    int arrayIndex = 0;

    fr = fopen (filename, "r"); // Open the file for reading
    if (fr == NULL)
    {
        fprintf(stderr, "File %s does not exist", filename);
        fileContent.hasError = true;
        return fileContent;
    }
    while(fgets(line, sizeof(line), fr) != NULL)   //get a word.  done if NULL
    {
        char *lineCopy;
        lineCopy = (char *) malloc(sizeof(line));
        strcpy(lineCopy, line);
        FileLine parsedLine = lineValidator(lineCopy, lineCounter++);
        parsedLine.originalLine = lineCopy;
        if (parsedLine.hasSyntaxError) {
            fileContent.hasError = true;
        }
        if (parsedLine.isEmptyOrComment == false) {
            fileContent.line[arrayIndex++] = parsedLine;
        }
    }
    fileContent.size = arrayIndex;
    fclose(fr);  /* close the file prior to exiting the routine */

    return fileContent;
}

