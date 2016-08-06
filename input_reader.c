#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include "input_reader.h"

bool isLabelValid(char* label){
    int i;
    if (strlen(label) > 0){ /* check label is not empty */
        if (isalpha(label[0])){ /*  Check first char is letter */
            for (i = 1; i < strlen(label); i++) {
                if ((!(isalpha(label[i])) && !(isdigit(label[i]))) ||  /*  All chars are digits or letters */
                    i == MAX_LABEL_SIZE) {  /* Size of Label is less than MAX_LABEL_SIZE (30) */
                    return false;
                }
            }
            return true;
        }
        else{
            return false; /* First Char in label is not a letter */
        }
    }
    else{
        return false; /* Label is 0 Chars */
    }
}

int getIntFromString(char *string){
    char* ptr;
    intmax_t num;
    if (strlen(string) == 0){ /* Check string is not empty */
        return INVALID_NUM_TOKEN;
    }
    num = strtoimax(string, &ptr, 10); /* cast string to number */
    if (num < MIN_NUM_SIZE || num > MAX_NUM_SIZE) { /* check number could be casted to 15 bits array */
        return INVALID_NUM_TOKEN;
    }
    if (strlen(ptr) != 0) { /* check if there was any other chars except for digits in the number */
        return INVALID_NUM_TOKEN;
    }
    return (int)num;
}

char* getNewSubStringFromString(const char* orig, int pos1, int pos2) {
    size_t sizeT;
    char* res;
    const char* pos;

    sizeT = (size_t)pos2 - pos1;
    res = (char*)malloc(sizeT);
    memset(res, 0, sizeT);
    pos = &orig[pos1] + 1;
    strncpy(res, pos, sizeT);
    res[sizeT - 1] = '\0';
    return res;
}

char* getNewStrBetweenTwoChars(const char* orig, char c1, char c2, bool checkEndOfLine, bool checkStartOfLine) {
    int c1Pos = -1;
    int c2Pos = -1;
    int i;
    char* string;

    for (i = 0; i < strlen(orig) ; i++) {
        if (c1Pos == -1 && c2Pos == -1 && orig[i] != c1){ /*  before word - did not find any quote yet */
            if (checkStartOfLine && orig[i] != '\0' && orig[i] != '\t' && orig[i] != ' '){
                return NULL;
            }
        }
        else if (c1Pos == -1 && c2Pos == -1 && orig[i] == c1){ /* got to first quotes */
            c1Pos = i;
        }
        else if (c1Pos > -1 && c2Pos == -1 && orig[i] == c2){ /* got second quote */
            c2Pos = i;
        }
        else if (checkEndOfLine && c1Pos > -1 && c2Pos > -1 && /*  after second quote */
                 (orig[i] != '\n' && orig[i] != '\r' && orig[i] != '\0' && orig[i] != '\t' && orig[i] != ' ')){
            return NULL;
        }
    }
    if (c2Pos > c1Pos && c1Pos > -1){
        string = getNewSubStringFromString(orig, c1Pos, c2Pos);
        return string;
    }
    return NULL;
}

char* getOperand(char* operandStr, Operand* operand) {
    int num;
    char* label;

    int operandStrSize;
    int openBracketPos;
    int closeBracketPos;
    int dashPos;
    int minNum;
    int maxNum;
    char* openBracketPosStr;
    char* closeBracketPosStr;
    char* dashPosStr;
    char* minNumStr;
    char* maxNumStr;

    operandStrSize = strlen(operandStr);
    openBracketPosStr = strchr(operandStr, '[');
    closeBracketPosStr = strchr(operandStr, ']');
    dashPosStr = strchr(operandStr, '-');

    /* Debug */
    /* fprintf(stderr, "Line is = %s\n", operandStr); */

    /* check direct addressing */
    if (operandStr[0] == '#') {
        num = getIntFromString(++operandStr);
        if (num != INVALID_NUM_TOKEN) { /* Miyadi */
            /* string is number */
            operand->addressingType = NUMBER;
            operand->value = num;
            return NULL;
        }
        else {
            return strcat("expected a number, but received ", operandStr);
        }
    }
    /*  Oger */
    else if (operandStr[0] == 'r' && strlen(operandStr) == 2) {
        if (operandStr[1] >= '0' && operandStr[1] <= '7') {
            operand->addressingType = REGISTER;
            operand->registerNum = (int)operandStr[1] - '0';
            return NULL;
        }
        else {
            return "Expected register number between 0 and 7, received register number out of bound";
        }
    }
    /* Dinami Yashir */
    else if (openBracketPosStr != NULL && closeBracketPosStr != NULL && dashPosStr != NULL) { /* Check we should be in Dinami Yashir */
        operand->addressingType = DYNAMIC;

        openBracketPos = (int) (openBracketPosStr - operandStr);
        closeBracketPos = (int) (closeBracketPosStr - operandStr);
        dashPos = (int) (dashPosStr - operandStr);

        if (closeBracketPos > dashPos && dashPos > openBracketPos && /* Check '[' then '-' then ']' */
            openBracketPos != 0 &&         /* Has label instead of [ */
            openBracketPos <= MAX_LABEL_SIZE && /* TODO: do we still need it? Label size is less then max size */
            closeBracketPos == operandStrSize - 1) { /* ']' is at the end*/

            label = getNewSubString(operandStr, openBracketPos);
            if (!isLabelValid(label)) {
                return strcat("Label is not valid - ", label);
            }
            minNumStr = getNewStrBetweenTwoChars(operandStr, '[', '-', false, false);
            if (minNumStr != NULL) {
                minNum = getIntFromString(minNumStr);
                free(minNumStr);
            }
            else {
                return "Failed Parsing Dynamic  addressing";
            }
            maxNumStr = getNewStrBetweenTwoChars(operandStr, '-', ']', false, false);
            if (maxNumStr != NULL) {
                maxNum = getIntFromString(maxNumStr);
                free(maxNumStr);
            }
            else {
                return "Failed Parsing Dynamic  addressing";
            }

            if (minNum == INVALID_NUM_TOKEN || minNum > MAX_DYNAMIC_OPERAND ||
                    maxNum == INVALID_NUM_TOKEN || maxNum > MAX_DYNAMIC_OPERAND) {
                return "Operand num is out of bound";
            }

            operand->label = label;
            operand->minNum = minNum;
            operand->maxNum = maxNum;
            return NULL;
        }
    }
    /* Yashir */
    else if (isLabelValid(operandStr)) {
        operand->addressingType = DIRECT;
        operand->label = copyString(operandStr);
        return NULL;
    }
    return "unknown addressing type error";
}

char* checkTwoOperands(char* rawOperandsString, FileLine* parsedLine){
    char* firstRawOperand;
    char* secondRawOperand;
    char* errString;

    firstRawOperand = strtok(rawOperandsString, " ,\t"); /* get first operand - split by comma and/or space */
    secondRawOperand = strtok(NULL, " ,\t"); /* get second operand - split by comma and/or space */

    if (secondRawOperand == NULL){
        return "Expected two operands, received one";
    }
    if (strtok(NULL, " ,\t") != NULL){
        return "Expected two operands, received more";
    }

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
    char* firstRawOperand;
    char* errString;

    firstRawOperand = strtok(rawOperandsString, " ,\t"); /* get first operand - split by comma and/or space */
    if (strtok(NULL, " ,\t") != NULL){ /* Operand 2 is not empty */
        return "Expected One operand - Received more";
    }
    else if (firstRawOperand == NULL){ /* Operand 1 is empty */
        return "Expected One operand - Received None";
    }

    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));
    errString = getOperand(firstRawOperand, parsedLine->firstOperValue);
    if (errString != NULL) return errString;

    parsedLine->numOfCommandOprands = 1;
    return NULL;
}

char* checkNoOperand(char* rawOperandsString, FileLine* parsedLine){
    char* firstRawOperand;
    firstRawOperand = strtok(rawOperandsString, " ,\t");
    if (firstRawOperand != NULL){ /* Operand 1 is not empty */
        return "Expected no operand - Received more";
    }
    parsedLine->numOfCommandOprands = 0;
    return NULL;
}

char* checkDataOperand(char* rawOperandsString, FileLine* parsedLine) {
    int dataSize = 1;
    int i;
    int* data;
    char* numString;
    int numberInt;
    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));

    for (i = 1; i < strlen(rawOperandsString) - 1; i++){
        if (rawOperandsString[i] == ',') {
            dataSize++;
        }
    }
    data = (int*)malloc(dataSize * sizeof(int));
    numString = strtok(rawOperandsString, " ,\t\n\r");
    for (i=0; i < dataSize; i++){
        numberInt = getIntFromString(numString);
        if (numberInt != INVALID_NUM_TOKEN){
            data[i] = numberInt;
            numString = strtok(NULL, " ,\t\n\r");
        }
        else {
            return "Data integer is out of bounds";
        }
    }
    parsedLine->firstOperValue->data = data;
    parsedLine->firstOperValue->dataSize = dataSize;
    return NULL;
}

char* checkStringOperand(char* rawOperandString, FileLine* parsedLine) {
    char* string;

    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));

    string = getNewStrBetweenTwoChars(rawOperandString, '"', '"', true, true);
    if (string == NULL)
        return "Failed parsing data string";

    parsedLine->firstOperValue->string = string;
    return NULL;
    /*
    for (i = 0; i < strlen(rawOperandString) ; i++) {
        if (firstQuotesLocation == -1 && secondQuotesLocation == -1 && rawOperandString[i] != '"'){ //  before word - did not find any quote yet
            if (rawOperandString[i] != '\0' && rawOperandString[i] != '\t' && rawOperandString[i] != ' '){
                return "String does not start with double quotes";
            }
        }
        else if (firstQuotesLocation == -1 && secondQuotesLocation == -1 && rawOperandString[i] == '"'){ // got to first quotes
            firstQuotesLocation = i;
        }
        else if (firstQuotesLocation > -1 && secondQuotesLocation == -1 && rawOperandString[i] == '"'){ // got second quote
            secondQuotesLocation = i;
        }
        else if (firstQuotesLocation > -1 && secondQuotesLocation > -1 && /*  after second quote
                (rawOperandString[i] != '\n' && rawOperandString[i] != '\r' && rawOperandString[i] != '\0' && rawOperandString[i] != '\t' && rawOperandString[i] != ' ')){
            return "String has data after closing double quotes";
        }
    }
    if (secondQuotesLocation > firstQuotesLocation && firstQuotesLocation > -1){
        stringSize = (size_t)secondQuotesLocation - firstQuotesLocation - 1;
        string = (char*) malloc(stringSize);
        strncpy(string, //  set string
                strchr(rawOperandString, '"') + 1,   //the string after the first double quotes
                secondQuotesLocation - firstQuotesLocation - 1); //  number of char to copy

        parsedLine->firstOperValue->string = string;
        return NULL;
    }
    return "String should be enclosed by double quotes and be more than one char";
    */
}

char* checkExternOrEntryOperand(char* rawOperandString, FileLine* parsedLine) {
    char* label;
    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));

    label = strtok(rawOperandString, " \t\n\r");

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
    else { /* Command is not known */
        errStr = "Unknown command";
    }
    return errStr;
}

char* lineValidator(FileLine* parsedLine) {
    char* string;
    char* parsedLabel;
    char* rawOperandsString;
    size_t strSize;
    char* lineToCheck = copyString(parsedLine->originalLine);
    string = strtok(lineToCheck, " \t\n\r"); /* Get first string */

    /* Handle empty lines */
    if (string == NULL){
        parsedLine->isEmptyOrComment = true;
        free(lineToCheck);
        return NULL;
    }/* Handle Comment Lines */
    else if (strlen(string) >= 1 && string[0] == ';'){
        parsedLine->isEmptyOrComment = true;
        free(lineToCheck);
        return NULL;
    }

    /* Handle labels */
    strSize = strlen(string);
    if (string[strSize - 1] == ':') { /* Label Found */
        parsedLabel = getNewSubString(string, strSize - 1);
        if (isLabelValid(parsedLabel)){
            parsedLine->label = parsedLabel;
        }
        else {
            free(lineToCheck);
            free(parsedLabel);
            return "Invalid label";
        }
        string = strtok(NULL, " \t\n\r"); /* Get action string */
    }

    parsedLine->action = copyString(string);      /* get the action */
    rawOperandsString = strtok(NULL, "\n\r"); /*  get remaining of line */
    char* errStr = validateActionAndOperands(rawOperandsString, parsedLine);
    free(lineToCheck);
    return errStr;
}

bool getFileContent(char* filename, FileContent* fileContent) {
    FILE * fr;
    char line[MAX_LINE_SIZE];
    int lineCounter = 1;
    int arrayIndex = 0;
    bool isFileOK = true;
    char* errString;
    FileLine* parsedLine;

    fr = fopen (filename, "r"); /*  Open the file for reading */
    if (fr == NULL) {
        fprintf(stderr, "File %s does not exist\n", filename);
        return false;
    }
    while(fgets(line, sizeof(line), fr) != NULL)   /* get a word.  done if NULL */
    {
        /* Debug */
        /* fprintf(stderr, "Line is -->  %s", line); */
        parsedLine = &fileContent->line[arrayIndex];
        memset(parsedLine, 0, sizeof(FileLine));
        parsedLine->originalLine = (char *) malloc(sizeof(line));
        if (parsedLine->originalLine == NULL){
            /* TODO: allocation error */
            exit(1);
        }
        memset(parsedLine->originalLine, 0, strlen(parsedLine->originalLine));
        strcpy(parsedLine->originalLine, line);

        errString = lineValidator(parsedLine);
        if (errString != NULL) {
            isFileOK = false;
            PrintSyntaxError(errString, lineCounter);
        }
        if (parsedLine->isEmptyOrComment == false) {
            arrayIndex++;
        }
        lineCounter++;
    }
    fileContent->size = arrayIndex;
    fclose(fr);  /* close the file prior to exiting the routine */

    return isFileOK;
}

bool initFileContent(FileContent** fileContent) {
    *fileContent = (FileContent*)malloc(sizeof(FileContent));
    memset(*fileContent, 0, sizeof(FileContent));
    if (fileContent == NULL)
        return false;

    (*fileContent)->line = (FileLine*)malloc(sizeof(FileLine)*MAX_FILE_LINES);
    if ((*fileContent)->line == NULL)
        return false;
    return true;
}

void freeFileContent(FileContent** fileContent) {
    int i;
    if ((*fileContent)->size > 0){
        printf("Clears file line");
        for (i = 0; i < (*fileContent)->size; ++i) {
            free((*fileContent)->line[i].label);
            free((*fileContent)->line[i].originalLine);
            free((*fileContent)->line[i].action);
            if ((*fileContent)->line[i].firstOperValue != NULL) {
                if ((*fileContent)->line[i].firstOperValue->label != NULL)
                    free((*fileContent)->line[i].firstOperValue->label);
                if ((*fileContent)->line[i].firstOperValue->data != NULL)
                    free((*fileContent)->line[i].firstOperValue->data);
                if ((*fileContent)->line[i].firstOperValue->entryOrExtern != NULL)
                    free((*fileContent)->line[i].firstOperValue->entryOrExtern);
                if ((*fileContent)->line[i].firstOperValue->string != NULL)
                    free((*fileContent)->line[i].firstOperValue->string);
            }
            free((*fileContent)->line[i].firstOperValue);
            if ((*fileContent)->line[i].secondOperValue != NULL) {
                if ((*fileContent)->line[i].secondOperValue->label != NULL)
                    free((*fileContent)->line[i].secondOperValue->label);
                if ((*fileContent)->line[i].secondOperValue->data != NULL)
                    free((*fileContent)->line[i].secondOperValue->data);
                if ((*fileContent)->line[i].secondOperValue->entryOrExtern != NULL)
                    free((*fileContent)->line[i].secondOperValue->entryOrExtern);
                if ((*fileContent)->line[i].secondOperValue->string != NULL)
                    free((*fileContent)->line[i].secondOperValue->string);
            }
            free((*fileContent)->line[i].secondOperValue);
        }
    }
    free((*fileContent)->line);
    free(*fileContent);
}

