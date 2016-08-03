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

char* getOperand(char* operandStr, Operand* operand) {
    char* openBracketLocationString = strchr(operandStr, '[');
    char* closeBracketLocationString = strchr(operandStr, ']');

    //check direct addressing
    if (operandStr[0] == '#') {
        int num = getIntFromString(++operandStr);
        if (num != INVALID_NUM_TOKEN) { //Miyadi
            //string is number
            operand->addressingType = NUMBER;
            operand->value = num;
            return NULL;
        }
        else {
            return strcat("expected a number, but received ", operandStr);
        }
    }
    // Oger
    else if (operandStr[0] == 'r' && strlen(operandStr) == 2) {
        if (operandStr[1] >= '0' && operandStr[1] <= '7') {
            operand->addressingType = REGISTER;
            operand->registerNum = (int)operandStr[1] - '0';
            return NULL;
        }
        else {
            strcat("Expected register number between 0 and 7, received - ", operandStr[1]);
        }
    }
    //Dinami Yashir
    else if (openBracketLocationString != NULL && closeBracketLocationString != NULL) { //Check we should be in Dinami Yashir
        operand->addressingType = DYNAMIC;
        if (closeBracketLocationString > openBracketLocationString &&
                operandStr[0] != '[' && //Has label instead of [
                strchr(operandStr, '[') - operandStr <= MAX_LABEL_SIZE && //TODO: do we still need it?
                operandStr[strlen(operandStr) - 1] == ']') { //Check '[' before ']', '[' not in first char, ']' is at the end, Label size is less then max size
            char* label = (char*)malloc(strchr(operandStr, '[') - operandStr);
            memcpy(label, operandStr, strchr(operandStr, '[') - operandStr); // copy label - everything before '['
            if (!isLabelValid(label)) {
                return strcat("Label is not valid - ", label);
            }
            operand->label = label;

            // get min number
            size_t minDashLocation = strchr(openBracketLocationString, '-') - openBracketLocationString; //get the char num where - exists
            char* minNum = (char*)malloc(minDashLocation - 1);
            strncpy(minNum, openBracketLocationString + 1, minDashLocation - 1);
            int numToCheck = getIntFromString(minNum);
            if (numToCheck != INVALID_NUM_TOKEN ||
                numToCheck > MAX_DYNAMIC_OPERAND) { //Can't be more than the num 13
                operand->minNum = numToCheck;
            }
            else {
                return "Operand num is out of bound";
            }
            // get max number
            char *maxDashLocationString = strchr(openBracketLocationString, '-');
            char* maxNum = (char*)malloc(strlen(maxDashLocationString) - 2);
            strncpy(maxNum, maxDashLocationString + 1, strlen(maxDashLocationString) - 2);
            int numToCheckMax = getIntFromString(maxNum);
            if (numToCheckMax != INVALID_NUM_TOKEN ||
                numToCheckMax > MAX_DYNAMIC_OPERAND) { //Can't be more than the num 13
                operand->maxNum = numToCheckMax;
                return NULL;
            }
            else {
                return "Operand num is out of bound";
            }
        }
    }
    //Yashir
    else if (isLabelValid(operandStr)) {
        operand->addressingType = DIRECT;
        operand->label = operandStr;
        return NULL;
    }
    else {
        return "unknown addressing type error";
    }
}

char* checkTwoOperands(char* rawOperandsString, FileLine* parsedLine){
    char* firstRawOperand = strtok(rawOperandsString, " ,\t"); //get first operand - split by comma and/or space
    char* secondRawOperand = strtok(NULL, " ,\t"); //get second operand - split by comma and/or space
    if (secondRawOperand == NULL){
        return "Expected two operands, received one";
    }
    if (strtok(NULL, " ,\t") != NULL){
        return "Expected two operands, received more";
    }

    char* errString;

    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));
    errString = getOperand(firstRawOperand, parsedLine->firstOperValue);
    if (errString != NULL) return errString;

    parsedLine->secondOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->secondOperValue, 0, sizeof(Operand));
    errString = getOperand(secondRawOperand, parsedLine->secondOperValue);
    if (errString != NULL) return errString;

    parsedLine->numOfCommandOprands = 2;
    return NULL;
}

char* checkOneOperand(char* rawOperandsString, FileLine* parsedLine){
    char* firstRawOperand = strtok(rawOperandsString, " ,\t"); //get first operand - split by comma and/or space
    if (strtok(NULL, " ,\t") != NULL){ //Operand 2 is not empty
        return "Expected One operand - Received more";
    }
    else if (firstRawOperand == NULL){ //Operand 1 is empty
        return "Expected One operand - Received None";
    }

    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));
    char* errString = getOperand(firstRawOperand, parsedLine->firstOperValue);
    if (errString != NULL) return errString;

    parsedLine->numOfCommandOprands = 1;
    return NULL;
}

char* checkNoOperand(char* rawOperandsString, FileLine* parsedLine){
    char* firstRawOperand = strtok(rawOperandsString, " ,\t");
    if (firstRawOperand != NULL){ //Operand 1 is not empty
        return "Expected no operand - Received more";
    }
    parsedLine->numOfCommandOprands = 0;
    return NULL;
}

char* checkDataOperand(char* rawOperandsString, FileLine* parsedLine) {
    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));
    int dataSize = 1;
    for (int i = 1; i < strlen(rawOperandsString) - 1; i++){
        if (rawOperandsString[i] == ',') {
            dataSize++;
        }
    }
    int* data = (int*)malloc(dataSize * sizeof(int));
    char* numString = strtok(rawOperandsString, " ,\t\n");
    int numberInt;
    for (int i=0; i < dataSize; i++){
        numberInt = getIntFromString(numString);
        if (numberInt != INVALID_NUM_TOKEN){
            data[i] = numberInt;
            numString = strtok(NULL, " ,\t\n");
        }
        else {
            char* errMsg = strcat(strcat("Data integer - ", numString), " is out of bounds");
            return errMsg;
        }
    }
    parsedLine->firstOperValue->data = data;
    parsedLine->firstOperValue->dataSize = dataSize;
    return NULL;
}

char* checkStringOperand(char* rawOperandString, FileLine* parsedLine) {
    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));
    int firstQuotesLocation = -1;
    int secondQuotesLocation = -1;
    for (int i = 0; i < strlen(rawOperandString) ; i++) {
        if (firstQuotesLocation == -1 && secondQuotesLocation == -1 && rawOperandString[i] != '"'){ // before word
            if (rawOperandString[i] != NULL || rawOperandString[i] != '\t' || rawOperandString[i] != ' '){
                return "String does not start with double quotes";
            }
        }
        else if (firstQuotesLocation == -1 && secondQuotesLocation == -1 && rawOperandString[i] == '"'){ //got to first quotes
            firstQuotesLocation = i;
        }
        else if (firstQuotesLocation > -1 && secondQuotesLocation == -1 && rawOperandString[i] == '"'){ //got second quote
            secondQuotesLocation = i;
        }
        else if (firstQuotesLocation > -1 && secondQuotesLocation > -1 && // after second quote
                (rawOperandString[i] != '\n' || rawOperandString[i] != NULL || rawOperandString[i] != '\t' || rawOperandString[i] != ' ')){
            return "String has data after closing double quotes";
        }
    }
    if (secondQuotesLocation > firstQuotesLocation && firstQuotesLocation > -1){
        size_t stringSize = (size_t)secondQuotesLocation - firstQuotesLocation - 1;
        char* string = (char*) malloc(stringSize);
        strncpy(string, // set string
                strchr(rawOperandString, '"') + 1, // the string after the first double quotes
                secondQuotesLocation - firstQuotesLocation - 1); // number of char to copy

        parsedLine->firstOperValue->string = string;
        return NULL;
    }
    return "String should be enclosed by double quotes and be more than one char";
}

char* checkExternOrEntryOperand(char* rawOperandString, FileLine* parsedLine) {
    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));
    char* label = strtok(rawOperandString, " \t\n");
    if (isLabelValid(label)){
        parsedLine->firstOperValue->entryOrExtern = label;
        return NULL;
    }
    return "Label is not valid";
}

char* validateActionAndOperands(char* rawOperandsString, FileLine* parsedLine) {
    char* errStr = NULL;
    char* action = parsedLine->action;
    if (strcmp(action, "mov") == 0) {
        parsedLine->actionType = MOV;
        errStr = checkTwoOperands(rawOperandsString, parsedLine);
        if (errStr == NULL && (parsedLine->secondOperValue->addressingType == NUMBER ||
                parsedLine->secondOperValue->addressingType == DYNAMIC))
            errStr = "Illegal Destination Operand Addressing on: mov command, allowed addressing types are DIRECT(1), REGISTER(3).";
    }
    else if (strcmp(action, "cmp") == 0){
        parsedLine->actionType = CMP;
        errStr = checkTwoOperands(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "add") == 0){
        parsedLine->actionType = ADD;
        errStr = checkTwoOperands(rawOperandsString, parsedLine);
        if (errStr == NULL && (parsedLine->secondOperValue->addressingType == NUMBER ||
                               parsedLine->secondOperValue->addressingType == DYNAMIC))
            errStr = "Illegal Destination Operand Addressing on: add command, allowed addressing types are DIRECT(1), REGISTER(3).";
    }
    else if (strcmp(action, "sub") == 0){
        parsedLine->actionType = SUB;
        errStr = checkTwoOperands(rawOperandsString, parsedLine);
        if (errStr == NULL && (parsedLine->secondOperValue->addressingType == NUMBER ||
                               parsedLine->secondOperValue->addressingType == DYNAMIC))
            errStr = "Illegal Destination Operand Addressing on: sub command, allowed addressing types are DIRECT(1), REGISTER(3).";
    }
    else if (strcmp(action, "lea") == 0){
        parsedLine->actionType = LEA;
        errStr = checkTwoOperands(rawOperandsString, parsedLine);

        if (errStr == NULL && parsedLine->firstOperValue->addressingType != DIRECT)
            errStr = "Illegal Source Operand Addressing on: lea command, allowed addressing type is DIRECT(1).";

        if (errStr == NULL && (parsedLine->secondOperValue->addressingType == NUMBER ||
                               parsedLine->secondOperValue->addressingType == DYNAMIC))
            errStr = "Illegal Destination Operand Addressing on: lea command, allowed addressing types are DIRECT(1), REGISTER(3).";
    }
    else if (strcmp(action, "clr") == 0){
        parsedLine->actionType = CLR;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
        if (errStr == NULL && (parsedLine->firstOperValue->addressingType == NUMBER ||
                               parsedLine->firstOperValue->addressingType == DYNAMIC))
            errStr = "Illegal Destination Operand Addressing on: clr command, allowed addressing types are DIRECT(1), REGISTER(3).";
    }
    else if (strcmp(action, "not") == 0){
        parsedLine->actionType = NOT;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
        if (errStr == NULL && (parsedLine->firstOperValue->addressingType == NUMBER ||
                               parsedLine->firstOperValue->addressingType == DYNAMIC))
            errStr = "Illegal Destination Operand Addressing on: not command, allowed addressing types are DIRECT(1), REGISTER(3).";
    }
    else if (strcmp(action, "inc") == 0){
        parsedLine->actionType = INC;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
        if (errStr == NULL && (parsedLine->firstOperValue->addressingType == NUMBER ||
                               parsedLine->firstOperValue->addressingType == DYNAMIC))
            errStr = "Illegal Destination Operand Addressing on: inc command, allowed addressing types are DIRECT(1), REGISTER(3).";
    }
    else if (strcmp(action, "dec") == 0){
        parsedLine->actionType = DEC;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
        if (errStr == NULL && (parsedLine->firstOperValue->addressingType == NUMBER ||
                               parsedLine->firstOperValue->addressingType == DYNAMIC))
            errStr = "Illegal Destination Operand Addressing on: dec command, allowed addressing types are DIRECT(1), REGISTER(3).";
    }
    else if (strcmp(action, "jmp") == 0){
        parsedLine->actionType = JMP;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
        if (errStr == NULL && (parsedLine->firstOperValue->addressingType == NUMBER ||
                               parsedLine->firstOperValue->addressingType == DYNAMIC))
            errStr = "Illegal Destination Operand Addressing on: jmp command, allowed addressing types are DIRECT(1), REGISTER(3).";
    }
    else if (strcmp(action, "bne") == 0){
        parsedLine->actionType = BNE;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
        if (errStr == NULL && (parsedLine->firstOperValue->addressingType == NUMBER ||
                               parsedLine->firstOperValue->addressingType == DYNAMIC))
            errStr = "Illegal Destination Operand Addressing on: bne command, allowed addressing types are DIRECT(1), REGISTER(3).";
    }
    else if (strcmp(action, "red") == 0){
        parsedLine->actionType = RED;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
        if (errStr == NULL && (parsedLine->firstOperValue->addressingType == NUMBER ||
                               parsedLine->firstOperValue->addressingType == DYNAMIC))
            errStr = "Illegal Destination Operand Addressing on: red command, allowed addressing types are DIRECT(1), REGISTER(3).";
    }
    else if (strcmp(action, "prn") == 0){
        parsedLine->actionType = PRN;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "jsr") == 0){
        parsedLine->actionType = JSR;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
        if (errStr == NULL && (parsedLine->firstOperValue->addressingType == NUMBER ||
                               parsedLine->firstOperValue->addressingType == DYNAMIC))
            errStr = "Illegal Destination Operand Addressing on: jsr command, allowed addressing types are DIRECT(1), REGISTER(3).";
    }
    else if (strcmp(action, "rts") == 0){
        parsedLine->actionType = RTS;
        checkNoOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "stop") == 0){
        parsedLine->actionType = STOP;
        errStr = checkNoOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, ".data") == 0){
        parsedLine->actionType = DATA;
        errStr = checkDataOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, ".string") == 0){
        parsedLine->actionType = STRING;
        errStr = checkStringOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, ".entry") == 0){
        parsedLine->actionType = ENTRY;
        errStr = checkExternOrEntryOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, ".extern") == 0){
        parsedLine->actionType = EXTERN;
        errStr = checkExternOrEntryOperand(rawOperandsString, parsedLine);
    }
    else { //Command is not known
        errStr = "Unknown command";
    }
    return errStr;
}

char* lineValidator(FileLine* parsedLine) {
    char* lineToCheck = (char*)malloc(strlen(parsedLine->originalLine));
    strcpy(lineToCheck, parsedLine->originalLine);
    char* string = strtok(lineToCheck, " \t\n"); //Get first string

    //Handle empty lines
    if (string == NULL){
        parsedLine->isEmptyOrComment = true;
        return NULL;
    }//Handle Comment Lines
    else if (strlen(string) >= 1 && string[0] == ';'){
        parsedLine->isEmptyOrComment = true;
        return NULL;
    }

    //Handle labels
    size_t strSize = strlen(string);
    if (string[strSize - 1] == ':') { //Label Found

        if (strSize > MAX_LABEL_SIZE + 1){ //TODO: should we keep this????
            return strcat("label size is more than allowed - ", MAX_LABEL_SIZE);
        }
        char* parsedLabel = (char*) malloc(strSize - 1);
        memcpy(parsedLabel, string, strSize - 1);  //get the label

        if (isLabelValid(parsedLabel)){
            parsedLine->label = parsedLabel;
        }
        else {
            return "Invalid label";
        }

        string = strtok(NULL, " \t\n"); //Get action string
        strSize = strlen(string);

    }

    char* actionStr = (char*) malloc(strSize);
    strcpy(actionStr, string);
    parsedLine->action = actionStr;      //get the action

    char* rawOperandsString = strtok(NULL, "\n"); // get remaining of line

    validateActionAndOperands(rawOperandsString, parsedLine);

    return NULL;
}

bool getFileContent(char* filename, FileContent* fileContent) {
    FILE * fr;
    char line[MAX_LINE_SIZE];
    int lineCounter = 1;
    int arrayIndex = 0;

    fr = fopen (filename, "r"); // Open the file for reading
    if (fr == NULL)
    {
        fprintf(stderr, "File %s does not exist", filename);
        return false;
    }
    while(fgets(line, sizeof(line), fr) != NULL)   //get a word.  done if NULL
    {
        char* lineCopy = (char *) malloc(sizeof(line));
        strcpy(lineCopy, line);

        FileLine* parsedLine = (FileLine*) malloc(sizeof(FileLine));
        memset(parsedLine, 0, sizeof(FileLine));
        parsedLine->originalLine = lineCopy;
        parsedLine->lineNumber = lineCounter;

        char* errString = lineValidator(parsedLine);
        if (errString != NULL) {
            PrintSyntaxError(errString, lineCounter);
        }
        if (parsedLine->isEmptyOrComment == false) {
            fileContent->line[arrayIndex++] = *parsedLine;
        }
        lineCounter++;
    }
    fileContent->size = arrayIndex;
    fclose(fr);  /* close the file prior to exiting the routine */

    return true;
}

