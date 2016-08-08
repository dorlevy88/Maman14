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
#define MAX_DYNAMIC_OPERAND 14 /* max exact number */
#define MAX_DYNAMIC_RANGE 13

/* **********************************************************************
 *
 *     FILE HANDLERS INTERNAL METHODS
 *
 * **********************************************************************/

char* isLabelValid(char* label) {
    int i;
    if (strlen(label) <= 0) return ERR_INVAILD_LABEL_EMPTY;
    if (isalpha(label[0]) == false) return ERR_INVAILD_LABEL_FIRST;
    if (strlen(label) >= MAX_LABEL_SIZE) return ERR_INVAILD_LABEL_MAX;

    for (i = 1; i < strlen(label); i++) {
        if (!isalpha(label[i]) && !isdigit(label[i])) {
            return ERR_INVAILD_LABEL_ILLEGAL;
        }
    }
    return NULL; /* Valid label */
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

    char* errStr;

    operandStrSize = strlen(operandStr);
    openBracketPosStr = strchr(operandStr, '[');
    closeBracketPosStr = strchr(operandStr, ']');
    dashPosStr = strchr(operandStr, '-');

    openBracketPos = (int) (openBracketPosStr - operandStr);
    closeBracketPos = (int) (closeBracketPosStr - operandStr);
    dashPos = (int) (dashPosStr - operandStr);

    if (closeBracketPos > dashPos && dashPos > openBracketPos && /* Check '[' then '-' then ']' */
        openBracketPos != 0 &&         /* Has label instead of [ */
        closeBracketPos == operandStrSize - 1) { /* ']' is at the end*/

        labelTemp = getNewSubString(operandStr, openBracketPos);
        errStr = isLabelValid(labelTemp);
        if (errStr != NULL) {
            return errMessage(errStr, labelTemp);
        }
        minNumStr = getNewStrBetweenTwoChars(operandStr, '[', '-', false, false);
        if (minNumStr != NULL) {
            minNumTemp = getIntFromString(minNumStr);
            if (minNumTemp == INVALID_NUM_TOKEN || minNumTemp > MAX_DYNAMIC_OPERAND) {
                errStr = errMessage(ERR_NUM_OUT_OF_BOUNDS, minNumStr);
                free(minNumStr);
                return errStr;
            }
            free(minNumStr);
        }
        else {
            return errMessage(ERR_BAD_DYNAMIC_ADDRESSING, operandStr);
        }
        maxNumStr = getNewStrBetweenTwoChars(operandStr, '-', ']', false, false);
        if (maxNumStr != NULL) {
            maxNumTemp = getIntFromString(maxNumStr);
            if (maxNumTemp == INVALID_NUM_TOKEN || maxNumTemp > MAX_DYNAMIC_OPERAND) {
                errStr = errMessage(ERR_NUM_OUT_OF_BOUNDS, maxNumStr);
                free(minNumStr);
                return errStr;
            }
            free(maxNumStr);
        }
        else {
            return errMessage(ERR_BAD_DYNAMIC_ADDRESSING, operandStr);
        }

        if (minNumTemp > maxNumTemp) {
            return copyString(ERR_LEFT_RIGHT_BOUNDS);
        }
        if (maxNumTemp - minNumTemp >= MAX_DYNAMIC_RANGE) {
            return copyString(ERR_LEFT_RIGHT_RANGE);
        }

        *label = labelTemp;
        *minNum = minNumTemp;
        *maxNum = maxNumTemp;
        return NULL;
    }
    return errMessage(ERR_BAD_DYNAMIC_ADDRESSING, operandStr);
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
            return errMessage(ERR_INVALID_NUMBER, operandStr);
        }
    }
    /*  Oger */
    else if (operandStr[0] == 'r' && strlen(operandStr) == 2 && operandStr[1] >= '0' && operandStr[1] <= '7') {
            operand->addressingType = REGISTER;
            operand->registerNum = (int)operandStr[1] - '0';
            return NULL;
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
    else if (isLabelValid(operandStr) == NULL) {
        operand->addressingType = DIRECT;
        operand->label = copyString(operandStr);
        return NULL;
    }
    return errMessage(ERR_UNKNOWN_ADDRESSING, operandStr);
}

char* checkTwoOperands(char* rawOperandsString, FileLine* parsedLine, bool checkAddressing){
    char* firstRawOperand;
    char* secondRawOperand;
    char* errString;

    if (rawOperandsString == NULL) {
        return errMessage(ERR_TWO_OP_GOT_NONE, parsedLine->action);
    }

    firstRawOperand = strtok(rawOperandsString, " ,\t"); /* get first operand - split by comma and/or space */
    secondRawOperand = strtok(NULL, " ,\t"); /* get second operand - split by comma and/or space */

    if (secondRawOperand == NULL){
        return errMessage(ERR_TWO_OP_GOT_ONE, rawOperandsString);
    }
    if (strtok(NULL, " ,\t") != NULL){
        return errMessage(ERR_TWO_OP_GOT_MORE, rawOperandsString);
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

    if (checkAddressing && (parsedLine->secondOperValue->addressingType == NUMBER ||
            parsedLine->secondOperValue->addressingType == DYNAMIC))
        return errMessage(ERR_ILLEGAL_DEST_ADDRESSING, parsedLine->action);

    return NULL;
}

char* checkOneOperand(char* rawOperandsString, FileLine* parsedLine, bool checkAddressing){
    char* firstRawOperand;
    char* errString;

    if (rawOperandsString == NULL) {
        return errMessage(ERR_ONE_OP_GOT_NONE, parsedLine->action);
    }

    firstRawOperand = strtok(rawOperandsString, " ,\t"); /* get first operand - split by comma and/or space */
    if (strtok(NULL, " ,\t") != NULL){ /* Operand 2 is not empty */
        return errMessage(ERR_ONE_OP_GOT_MORE, rawOperandsString);
    }
    else if (firstRawOperand == NULL){ /* Operand 1 is empty */
        return errMessage(ERR_ONE_OP_GOT_NONE, parsedLine->action);
    }

    parsedLine->numOfCommandOprands = 1;
    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));
    errString = getOperand(firstRawOperand, parsedLine->firstOperValue);
    if (errString != NULL) return errString;

    if (checkAddressing && (parsedLine->firstOperValue->addressingType == NUMBER ||
            parsedLine->firstOperValue->addressingType == DYNAMIC))
       return errMessage(ERR_ILLEGAL_DEST_ADDRESSING, parsedLine->action);

    return NULL;
}

char* checkNoOperand(char* rawOperandsString, FileLine* parsedLine){
    char* firstRawOperand;
    firstRawOperand = strtok(rawOperandsString, " ,\t");
    if (firstRawOperand != NULL){ /* Operand 1 is not empty */
        return errMessage(ERR_NO_OP_GOT_MORE, rawOperandsString);
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

    if (rawOperandsString == NULL) {
        return copyString(ERR_DATA_INVALID);
    }

    for (i = 1; i < strlen(rawOperandsString) - 1; i++){
        if (rawOperandsString[i] == ',') {
            dataSize++;
        }
    }
    data = (int*)malloc(dataSize * sizeof(int));
    numString = strtok(rawOperandsString, " ,\t\n\r");
    if (numString == NULL) {
        return copyString(ERR_DATA_INVALID);
    }
    for (i=0; i < dataSize; i++){
        numberInt = getIntFromString(numString);
        if (numberInt != INVALID_NUM_TOKEN){
            data[i] = numberInt;
            numString = strtok(NULL, " ,\t\n\r");
            if (numString == NULL && i < dataSize) {
                return copyString(ERR_DATA_INVALID);
            }
        }
        else {
            return errMessage(ERR_DATA_OUT_OF_BOUNDS, numString);
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

    if (rawOperandString == NULL) {
        return copyString(ERR_DATA_INVALID);
    }

    string = getNewStrBetweenTwoChars(rawOperandString, '"', '"', true, true);
    if (string == NULL)
        return errMessage(ERR_STRING_INVALID, rawOperandString);

    parsedLine->firstOperValue->string = string;
    return NULL;
}

char* checkExternOrEntryOperand(char* rawOperandString, FileLine* parsedLine) {
    char* label;
    char* errStr;
    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));

    if (rawOperandString == NULL) {
        return errMessage(ERR_ONE_OP_GOT_NONE, parsedLine->action);
    }

    label = strtok(rawOperandString, " \t\n\r");
    errStr = isLabelValid(label);
    if (errStr == NULL){
        parsedLine->firstOperValue->entryOrExtern = copyString(label);
        return NULL;
    }
    return errMessage(errStr, label);
}

char* validateActionAndOperands(char* rawOperandsString, FileLine* parsedLine) {
    char* errStr = NULL;
    char* action = parsedLine->action;
    if (strcmp(action, "mov") == 0) {
        parsedLine->actionType = MOV;
        errStr = checkTwoOperands(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "cmp") == 0){
        parsedLine->actionType = CMP;
        errStr = checkTwoOperands(rawOperandsString, parsedLine, false); /* For cmp command there is no problem with destination operand addressing*/
    }
    else if (strcmp(action, "add") == 0){
        parsedLine->actionType = ADD;
        errStr = checkTwoOperands(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "sub") == 0){
        parsedLine->actionType = SUB;
        errStr = checkTwoOperands(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "lea") == 0){
        parsedLine->actionType = LEA;
        errStr = checkTwoOperands(rawOperandsString, parsedLine, true);
        if (errStr == NULL && parsedLine->firstOperValue != NULL &&
                parsedLine->firstOperValue->addressingType != DIRECT)
            errStr = ERR_LEA_SOURCE_ADDRESSING;
    }
    else if (strcmp(action, "clr") == 0){
        parsedLine->actionType = CLR;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "not") == 0){
        parsedLine->actionType = NOT;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "inc") == 0){
        parsedLine->actionType = INC;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "dec") == 0){
        parsedLine->actionType = DEC;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "jmp") == 0){
        parsedLine->actionType = JMP;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "bne") == 0){
        parsedLine->actionType = BNE;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "red") == 0){
        parsedLine->actionType = RED;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "jsr") == 0){
        parsedLine->actionType = JSR;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "prn") == 0){
        parsedLine->actionType = PRN;
        errStr = checkOneOperand(rawOperandsString, parsedLine, false); /* For prn command there is no problem with destination operand addressing*/
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
        errStr = errMessage(ERR_UNKNOWN_CMD, action);
    }
    return errStr;
}

char* lineValidator(FileLine* parsedLine) {
    char* string;
    char* parsedLabel;
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
        errStr = isLabelValid(parsedLabel);
        if (errStr == NULL){
            parsedLine->label = parsedLabel;
        }
        else {
            free(lineToCheck);
            errStr = errMessage(errStr, parsedLabel);
            free(parsedLabel);
            return errStr;
        }
        string = strtok(NULL, " \t\n\r"); /* Get action string */
    }

    parsedLine->action = copyString(string);      /* get the action */
    string = strtok(NULL, "\n\r"); /*  get remaining of line */
    errStr = validateActionAndOperands(string, parsedLine);
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

        parsedLine->lineNumber = lineCounter;
        parsedLine->originalLine = copyString(line);
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

