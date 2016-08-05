#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>
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

char* getOperand(char* operandStr, Operand* operand) {
    size_t minDashLocation;
    char* openBracketLocationString;
    char* closeBracketLocationString;
    int num;
    char* label;
    char* minNum;
    char* maxNum;
    char* maxDashLocationString;
    int numToCheck;
    int numToCheckMax;

    openBracketLocationString = strchr(operandStr, '[');
    closeBracketLocationString = strchr(operandStr, ']');

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
    else if (openBracketLocationString != NULL && closeBracketLocationString != NULL) { /* Check we should be in Dinami Yashir */
        operand->addressingType = DYNAMIC;
        if (closeBracketLocationString > openBracketLocationString &&
                operandStr[0] != '[' && /* Has label instead of [ */
                strchr(operandStr, '[') - operandStr <= MAX_LABEL_SIZE && /* TODO: do we still need it? */
                operandStr[strlen(operandStr) - 1] == ']') { /* Check '[' before ']', '[' not in first char, ']' is at the end, Label size is less then max size */
            label = (char*)malloc(strchr(operandStr, '[') - operandStr);
            memcpy(label, operandStr, strchr(operandStr, '[') - operandStr); /*  copy label - everything before '[' */
            if (!isLabelValid(label)) {
                return strcat("Label is not valid - ", label);
            }
            operand->label = label;

            /*  get min number */
            minDashLocation = strchr(openBracketLocationString, '-') - openBracketLocationString; /* get the char num where - exists */
            minNum = (char*)malloc(minDashLocation - 1);
            strncpy(minNum, openBracketLocationString + 1, minDashLocation - 1);
            numToCheck = getIntFromString(minNum);
            if (numToCheck != INVALID_NUM_TOKEN ||
                numToCheck > MAX_DYNAMIC_OPERAND) { /* Can't be more than the num 13 */
                operand->minNum = numToCheck;
            }
            else {
                return "Operand num is out of bound";
            }
            /*  get max number */
            maxDashLocationString = strchr(openBracketLocationString, '-');
            maxNum = (char*)malloc(strlen(maxDashLocationString) - 2);
            strncpy(maxNum, maxDashLocationString + 1, strlen(maxDashLocationString) - 2);
            numToCheckMax = getIntFromString(maxNum);
            if (numToCheckMax != INVALID_NUM_TOKEN ||
                numToCheckMax > MAX_DYNAMIC_OPERAND) { /* Can't be more than the num 13 */
                operand->maxNum = numToCheckMax;
                return NULL;
            }
            else {
                return "Operand num is out of bound";
            }
        }
    }
    /* Yashir */
    else if (isLabelValid(operandStr)) {
        operand->addressingType = DIRECT;
        operand->label = operandStr;
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
    int firstQuotesLocation = -1;
    int secondQuotesLocation = -1;
    int i;
    char* string;
    size_t stringSize;

    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));
    for (i = 0; i < strlen(rawOperandString) ; i++) {
        if (firstQuotesLocation == -1 && secondQuotesLocation == -1 && rawOperandString[i] != '"'){ /*  before word - did not find any quote yet */
            if (rawOperandString[i] != '\0' || rawOperandString[i] != '\t' || rawOperandString[i] != ' '){
                return "String does not start with double quotes";
            }
        }
        else if (firstQuotesLocation == -1 && secondQuotesLocation == -1 && rawOperandString[i] == '"'){ /* got to first quotes */
            firstQuotesLocation = i;
        }
        else if (firstQuotesLocation > -1 && secondQuotesLocation == -1 && rawOperandString[i] == '"'){ /* got second quote */
            secondQuotesLocation = i;
        }
        else if (firstQuotesLocation > -1 && secondQuotesLocation > -1 && /*  after second quote */
                (rawOperandString[i] != '\n' && rawOperandString[i] != '\r' && rawOperandString[i] != '\0' && rawOperandString[i] != '\t' && rawOperandString[i] != ' ')){
            return "String has data after closing double quotes";
        }
    }
    if (secondQuotesLocation > firstQuotesLocation && firstQuotesLocation > -1){
        stringSize = (size_t)secondQuotesLocation - firstQuotesLocation - 1;
        string = (char*) malloc(stringSize);
        strncpy(string, /*  set string */
                strchr(rawOperandString, '"') + 1, /*  the string after the first double quotes */
                secondQuotesLocation - firstQuotesLocation - 1); /*  number of char to copy */

        parsedLine->firstOperValue->string = string;
        return NULL;
    }
    return "String should be enclosed by double quotes and be more than one char";
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
    char* actionStr;
    char* rawOperandsString;
    size_t strSize;
    char* lineToCheck = (char*)malloc(strlen(parsedLine->originalLine));
    strcpy(lineToCheck, parsedLine->originalLine);

    string = strtok(lineToCheck, " \t\n\r"); /* Get first string */

    /* Handle empty lines */
    if (string == NULL){
        parsedLine->isEmptyOrComment = true;
        return NULL;
    }/* Handle Comment Lines */
    else if (strlen(string) >= 1 && string[0] == ';'){
        parsedLine->isEmptyOrComment = true;
        return NULL;
    }

    /* Handle labels */
    strSize = strlen(string);
    if (string[strSize - 1] == ':') { /* Label Found */

        if (strSize > MAX_LABEL_SIZE + 1){ /* TODO: should we keep this???? */
            return "label size is more than allowed";
        }
        parsedLabel = (char*) malloc(strSize - 1);
        memcpy(parsedLabel, string, strSize - 1);  /* get the label */

        if (isLabelValid(parsedLabel)){
            parsedLine->label = parsedLabel;
        }
        else {
            return "Invalid label";
        }

        string = strtok(NULL, " \t\n\r"); /* Get action string */
        strSize = strlen(string);

    }

    actionStr = (char*) malloc(strSize);
    strcpy(actionStr, string);
    parsedLine->action = actionStr;      /* get the action */

    rawOperandsString = strtok(NULL, "\n\r"); /*  get remaining of line */

    return validateActionAndOperands(rawOperandsString, parsedLine);
}

bool getFileContent(char* filename, FileContent* fileContent) {
    FILE * fr;
    char line[MAX_LINE_SIZE];
    int lineCounter = 1;
    int arrayIndex = 0;
    bool isFileOK = true;
    char* lineCopy;
    char* errString;
    FileLine* parsedLine;

    fr = fopen (filename, "r"); /*  Open the file for reading */
    if (fr == NULL)
    {
        fprintf(stderr, "File %s does not exist\n", filename);
        return false;
    }
    while(fgets(line, sizeof(line), fr) != NULL)   /* get a word.  done if NULL */
    {
        lineCopy = (char *) malloc(sizeof(line));
        strcpy(lineCopy, line);
        /* Debug */
        /* fprintf(stderr, "Line is = %s\n", line); */
        parsedLine = (FileLine*) malloc(sizeof(FileLine));
        memset(parsedLine, 0, sizeof(FileLine));
        parsedLine->originalLine = lineCopy;
        parsedLine->lineNumber = lineCounter;

        errString = lineValidator(parsedLine);
        if (errString != NULL) {
            isFileOK = false;
            PrintSyntaxError(errString, lineCounter);
        }
        if (parsedLine->isEmptyOrComment == false) {
            fileContent->line[arrayIndex++] = *parsedLine;
        }
        lineCounter++;
    }
    fileContent->size = arrayIndex;
    fclose(fr);  /* close the file prior to exiting the routine */

    return isFileOK;
}

