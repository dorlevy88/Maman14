#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#include "input_reader.h"

#define MAX_FILE_LINES 1000
#define MAX_LINE_SIZE 80
#define MAX_LABEL_SIZE 30
#define MIN_NUM_SIZE -16384
#define MAX_NUM_SIZE 16384
#define INVALID_NUM_TOKEN -999999
#define MAX_DYNAMIC_OPERAND 13 /* max exact number */

/* **********************************************************************
 *
 *     FILE HANDLERS INTERNAL METHODS
 *
 * **********************************************************************/

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

char* checkDynamicAddressing (char* operandStr, char** label, int* minNum, int* maxNum) {
    int operandStrSize;
    int openBracketPos;
    int closeBracketPos;
    int dashPos;
    char* openBracketPosStr;
    char* closeBracketPosStr;
    char* dashPosStr;
    char* minNumStr;
    char* maxNumStr;

    int minNumTemp;
    int maxNumTemp;
    char* labelTemp;

    operandStrSize = strlen(operandStr);
    openBracketPosStr = strchr(operandStr, '[');
    closeBracketPosStr = strchr(operandStr, ']');
    dashPosStr = strchr(operandStr, '-');

    openBracketPos = (int) (openBracketPosStr - operandStr);
    closeBracketPos = (int) (closeBracketPosStr - operandStr);
    dashPos = (int) (dashPosStr - operandStr);

    if (closeBracketPos > dashPos && dashPos > openBracketPos && /* Check '[' then '-' then ']' */
        openBracketPos != 0 &&         /* Has label instead of [ */
        openBracketPos <= MAX_LABEL_SIZE && /* TODO: do we still need it? Label size is less then max size */
        closeBracketPos == operandStrSize - 1) { /* ']' is at the end*/

        labelTemp = getNewSubString(operandStr, openBracketPos);
        if (!isLabelValid(labelTemp)) {
            return strcat("Label is not valid - ", labelTemp);
        }
        minNumStr = getNewStrBetweenTwoChars(operandStr, '[', '-', false, false);
        if (minNumStr != NULL) {
            minNumTemp = getIntFromString(minNumStr);
            free(minNumStr);
        }
        else {
            return "Failed Parsing Dynamic  addressing";
        }
        maxNumStr = getNewStrBetweenTwoChars(operandStr, '-', ']', false, false);
        if (maxNumStr != NULL) {
            maxNumTemp = getIntFromString(maxNumStr);
            free(maxNumStr);
        }
        else {
            return "Failed Parsing Dynamic  addressing";
        }

        if (minNumTemp == INVALID_NUM_TOKEN || minNumTemp > MAX_DYNAMIC_OPERAND ||
                maxNumTemp == INVALID_NUM_TOKEN || maxNumTemp > MAX_DYNAMIC_OPERAND) {
            return "Operand num is out of bound";
        }

        *label = labelTemp;
        *minNum = minNumTemp;
        *maxNum = maxNumTemp;
        return NULL;
    }
    return "Incorrect dynamic addressing format";
}

char* getOperand(char* operandStr, Operand* operand) {
    int num;
    char* label;

    char* openBracketPosStr;
    char* closeBracketPosStr;
    char* dashPosStr;
    int minNum;
    int maxNum;
    char* errStr;

    openBracketPosStr = strchr(operandStr, '[');
    closeBracketPosStr = strchr(operandStr, ']');
    dashPosStr = strchr(operandStr, '-');

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
        errStr = checkDynamicAddressing(operandStr, &label, &minNum, &maxNum);
        if (errStr == NULL) {
            operand->addressingType = DYNAMIC;
            operand->label = label;
            operand->minNum = minNum;
            operand->maxNum = maxNum;
            return NULL;
        }
        else {
            return errStr;
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
        return ERR_TWO_OP_GOT_ONE;
    }
    if (strtok(NULL, " ,\t") != NULL){
        return ERR_TWO_OP_GOT_MORE;
    }

    parsedLine->numOfCommandOprands = 2;

    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));
    errString = getOperand(firstRawOperand, parsedLine->firstOperValue);
    if (errString != NULL) return errString;

    parsedLine->secondOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->secondOperValue, 0, sizeof(Operand));
    errString = getOperand(secondRawOperand, parsedLine->secondOperValue);
    if (errString != NULL) return errString;

    if (parsedLine->secondOperValue->addressingType == NUMBER || parsedLine->secondOperValue->addressingType == DYNAMIC)
        return ERR_ILLEGAL_DEST_ADDRESSING;

    return NULL;
}

char* checkOneOperand(char* rawOperandsString, FileLine* parsedLine){
    char* firstRawOperand;
    char* errString;

    firstRawOperand = strtok(rawOperandsString, " ,\t"); /* get first operand - split by comma and/or space */
    if (strtok(NULL, " ,\t") != NULL){ /* Operand 2 is not empty */
        return ERR_ONE_OP_GOT_MORE;
    }
    else if (firstRawOperand == NULL){ /* Operand 1 is empty */
        return ERR_ONE_OP_GOT_NONE;
    }

    parsedLine->numOfCommandOprands = 1;
    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));
    errString = getOperand(firstRawOperand, parsedLine->firstOperValue);
    if (errString != NULL) return errString;

    if (parsedLine->firstOperValue->addressingType == NUMBER || parsedLine->firstOperValue->addressingType == DYNAMIC)
       return ERR_ILLEGAL_DEST_ADDRESSING;

    return NULL;
}

char* checkNoOperand(char* rawOperandsString, FileLine* parsedLine){
    char* firstRawOperand;
    firstRawOperand = strtok(rawOperandsString, " ,\t");
    if (firstRawOperand != NULL){ /* Operand 1 is not empty */
        return ERR_NO_OP_GOT_MORE;
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
            return ERR_DATA_OUT_OF_BOUNDS;
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
        return ERR_STRING_INVALID;

    parsedLine->firstOperValue->string = string;
    return NULL;
}

char* checkExternOrEntryOperand(char* rawOperandString, FileLine* parsedLine) {
    char* label;
    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));

    label = strtok(rawOperandString, " \t\n\r");

    if (isLabelValid(label)){
        parsedLine->firstOperValue->entryOrExtern = copyString(label);
        return NULL;
    }
    return ERR_INVAILD_LABEL;
}

char* validateActionAndOperands(char* rawOperandsString, FileLine* parsedLine) {
    char* errStr = NULL;
    char* action = parsedLine->action;
    if (strcmp(action, "mov") == 0) {
        parsedLine->actionType = MOV;
        errStr = checkTwoOperands(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "cmp") == 0){
        parsedLine->actionType = CMP;
        errStr = checkTwoOperands(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "add") == 0){
        parsedLine->actionType = ADD;
        errStr = checkTwoOperands(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "sub") == 0){
        parsedLine->actionType = SUB;
        errStr = checkTwoOperands(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "lea") == 0){
        parsedLine->actionType = LEA;
        errStr = checkTwoOperands(rawOperandsString, parsedLine);
        if (errStr == NULL && parsedLine->firstOperValue != NULL &&
                parsedLine->firstOperValue->addressingType != DIRECT)
            errStr = ERR_LEA_SOURCE_ADDRESSING;
    }
    else if (strcmp(action, "clr") == 0){
        parsedLine->actionType = CLR;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "not") == 0){
        parsedLine->actionType = NOT;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "inc") == 0){
        parsedLine->actionType = INC;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "dec") == 0){
        parsedLine->actionType = DEC;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "jmp") == 0){
        parsedLine->actionType = JMP;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "bne") == 0){
        parsedLine->actionType = BNE;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "red") == 0){
        parsedLine->actionType = RED;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "jsr") == 0){
        parsedLine->actionType = JSR;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "prn") == 0){
        parsedLine->actionType = PRN;
        errStr = checkOneOperand(rawOperandsString, parsedLine);
        if (errStr != NULL && strcmp(errStr, ERR_ILLEGAL_DEST_ADDRESSING) == 0) /* For prn command there is no problem with destination operand*/
            errStr = NULL;
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
        parsedLine->actionType = UNKNOWN;
        errStr = ERR_UNKNOWN_CMD;
    }
    return errStr;
}

char* lineValidator(FileLine* parsedLine) {
    char* string;
    char* parsedLabel;
    char* rawOperandsString;
    size_t strSize;
    char* errStr;

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
            return ERR_INVAILD_LABEL;
        }
        string = strtok(NULL, " \t\n\r"); /* Get action string */
    }

    parsedLine->action = copyString(string);      /* get the action */
    rawOperandsString = strtok(NULL, "\n\r"); /*  get remaining of line */
    errStr = validateActionAndOperands(rawOperandsString, parsedLine);
    free(lineToCheck);
    return errStr;
}

/* **********************************************************************
 *
 *     FILE HANDLERS PUBLIC METHODS
 *
 * **********************************************************************/

/**
 * Return a struct full with the file data as needed for the transition pass
 * The status indicates if errors ha occured in the parsing procedure
 * @param filename
 * @param fileContent
 * @return Status (Pass/Fail)
 */
Status getFileContent(char *filename, FileContent *fileContent) {
    FILE * fr;
    char line[MAX_LINE_SIZE];
    int lineCounter = 1;
    int arrayIndex = 0;
    Status fileStatus = Pass;
    char* errString;
    FileLine* parsedLine;

    fr = fopen (filename, "r"); /*  Open the file for reading */
    if (fr == NULL) {
        printInternalError(ERR_FILE_NOT_FOUND, filename);
        return Fail;
    }
    fileContent->filename = copyString(filename);
    while(fgets(line, sizeof(line), fr) != NULL)   /* get a word.  done if NULL */
    {
        if(DEBUG) printf("Line is -->  %s", line);
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
            fileStatus = Fail;
            printSyntaxError(errString, filename, lineCounter);
        }
        if (parsedLine->isEmptyOrComment == false) {
            arrayIndex++;
        }
        lineCounter++;
    }
    fileContent->size = arrayIndex;
    fclose(fr);  /* close the file prior to exiting the routine */
    return fileStatus;
}

Status initFileContent(FileContent** fileContent) {
    *fileContent = (FileContent*)malloc(sizeof(FileContent));
    memset(*fileContent, 0, sizeof(FileContent));
    if (*fileContent == NULL)
        return Fail;

    (*fileContent)->line = (FileLine*)malloc(sizeof(FileLine)*MAX_FILE_LINES);
    if ((*fileContent)->line == NULL)
        return Fail;
    return Pass;
}

void freeFileContent(FileContent** fileContent) {
    int i;
    if ((*fileContent)->size > 0){
        if(DEBUG) printf("Clears file line\n");
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
    if ((*fileContent)->filename != NULL)
        free((*fileContent)->filename);
    free(*fileContent);
}

