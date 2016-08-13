#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>

#include "input_reader.h"

#define MAX_FILE_LINES 1000
#define MAX_LINE_SIZE 80
#define MAX_LABEL_SIZE 30
#define MAX_DATA_NUMBER 15
#define MAX_CMD_NUMBER 13
#define INVALID_NUM_TOKEN -999999
#define MAX_DYNAMIC_OPERAND 14 /* max exact number */
#define NUM_INVALID_LABELS 24
#define MAX_REGISTER_CHARS 2
#define MIN_REGISTER_NUM 0
#define MAX_REGISTER_NUM 7
const char* INVALID_LABELS[NUM_INVALID_LABELS] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
                                                  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

/* **********************************************************************
 *
 *     FILE HANDLERS INTERNAL METHODS
 *
 * **********************************************************************/

/**
 * Returns amount of commas (,) in the input string
 * @param string
 * @return int - number of commas in string
 */
int getNumCommas(char* string){
    int i, counter = 0;

    /* Iterate over all characters, increase counter when hit a comma */
    for (i = 0; i < strlen(string) ; ++i) {
        if (string[i] == ','){
            counter++;
        }
    }
    return counter;
}

/**
 * Returns True if a the input string is a valid label  - Qualifying the following conditions -
 *  1. String is not empty
 *  2. First char is an alpha numeric char
 *  3. Total string size is smaller equal 30 chars
 *  4. string is not a reserved word (mov, cmp ...)
 *  5. all chars are alpha numeric or digits
 * @param label - String to be qualified as label
 * @return char* errMsg - Null if label is valid, error message otherwise
 */
char* isLabelValid(char* label) {
    int i;
    const char** commands = INVALID_LABELS;

    if (strlen(label) <= 0) return ERR_INVAILD_LABEL_EMPTY; /* String is not empty */
    if (isalpha(label[0]) == false) return ERR_INVAILD_LABEL_FIRST; /* First char is an alpha numeric char */
    if (strlen(label) >= MAX_LABEL_SIZE) return ERR_INVAILD_LABEL_MAX; /* Total string size is smaller equal 30 chars */

    for (i = 0; i < NUM_INVALID_LABELS ; ++i) { /* string is not a reserved word (mov, cmp ...) */
        if (strcmp(label, commands[i]) == 0){
            return ERR_INVAILD_LABEL_ILLEGAL;
        }
    }

    for (i = 1; i < strlen(label); i++) { /* all chars are alpha numeric or digits */
        if (!isalpha(label[i]) && !isdigit(label[i])) {
            return ERR_INVAILD_LABEL_ILLEGAL;
        }
    }
    return NULL; /* Valid label */
}

/**
 * Returns int number when string qualifies as a number that can be represented in MaxBits length array of bits
 * Needs to meet the following rules -
 *  1. String is not empty
 *  2. Casted number could be casted again to MaxBits bit array
 *  3. The string was consisted of only numbers with no additional alpha numeric chars
 * @param string - char* to "cast" as a number
 * @param maxBits - length of bit array the number would be casted into
 * @return INVALID_NUM_TOKEN when rules are not met, integer casted from string when all rules met
 */
int getIntFromString(char *string, int maxBits){
    char* ptr;
    intmax_t num;
    const int max = (int)pow(2, maxBits - 1);

    if (strlen(string) == 0){ /* Check string is not empty */
        return INVALID_NUM_TOKEN;
    }
    num = strtoimax(string, &ptr, 10); /* cast string to number */
    if (num < (max * -1) || num >= max) { /* check number could be casted to 15/13 bits array */
        return INVALID_NUM_TOKEN;
    }
    if (strlen(ptr) != 0) { /* check if there was any other chars except for digits in the number */
        return INVALID_NUM_TOKEN;
    }
    return (int)num;
}

/**
 * Returns substring from ogirinal string
 * @param orig - char* that contains the full string to get the substring from
 * @param pos1 - Position for substring start
 * @param pos2 - Position for substring end
 * @return Substring from string from location pos1 to pos2
 */
char* getNewSubStringFromString(const char* orig, int pos1, int pos2) {
    size_t sizeT;
    char* res;
    const char* pos;

    sizeT = (size_t)pos2 - pos1; /* Find the size of substring */
    res = (char*)malloc(sizeT);  /* Allocate the size of substring */
    memset(res, 0, sizeT);
    pos = &orig[pos1] + 1; /* pos has the pointer to the beginning of substring */
    strncpy(res, pos, sizeT); /* copy the substring to res from string pos and the amount of chars - sizeT */
    res[sizeT - 1] = '\0';  /* set last char in res as end of string \0 */
    return res;
}

/**
 * Find if a substring exist in orig string between c1 and c2 with two options -
 *  1. check end of line - will control if to ignore chars after substring ends
 *  2. check start of line - will control if to ignore chars before substring begins
 * @param orig - char* - original string
 * @param c1 - char - Character which substring should begin with
 * @param c2 - char - Character which substring should end with
 * @param checkEndOfLine - Turn on/off whether to check for unknown chars after end of substring
 * @param checkStartOfLine - Turn on/off whether to check for unknown chars before start of substring
 * @return char* substring if substring exist in string and qualify with all the above checks, NULL otherwise
 */
char* getNewStrBetweenTwoChars(const char* orig, char c1, char c2, bool checkEndOfLine, bool checkStartOfLine) {
    int c1Pos = -1;
    int c2Pos = -1;
    int i;
    char* string;

    for (i = 0; i < strlen(orig) ; i++) {
        if (c1Pos == -1 && c2Pos == -1 && orig[i] != c1){ /*  before word - did not find c1 char or c2 char in string yet - Ignore tabs, spaces or NULL (\0) */
            if (checkStartOfLine && orig[i] != '\0' && orig[i] != '\t' && orig[i] != ' '){
                return NULL;
            }
        }
        else if (c1Pos == -1 && c2Pos == -1 && orig[i] == c1){ /* Starting word - found c1 char in string */
            c1Pos = i;
        }
        else if (c1Pos > -1 && c2Pos == -1 && orig[i] == c2){ /* Ending Word - Found c2 char in string */
            c2Pos = i;
        }
        else if (checkEndOfLine && c1Pos > -1 && c2Pos > -1 && /*  After word - Check no other chars exist except for spaces, tabs, end of lines chars (\n, \r) or NULL (\0) */
                 (orig[i] != '\n' && orig[i] != '\r' && orig[i] != '\0' && orig[i] != '\t' && orig[i] != ' ')){
            return NULL;
        }
    }
    if (c2Pos > c1Pos && c1Pos > -1){ /* Check that positions of starting char and ending char are valid - Char 2 is after char 1 and char 1 position exists */
        string = getNewSubStringFromString(orig, c1Pos, c2Pos); /* get the substring starting with c1 and end at c2 */
        return string;
    }
    return NULL;
}

/**
 * Function gets a dynamic addressing string, checks if it qualifies to be as such and if so, set the label, minNum and maxNum retrieved from the caller
 * Dynamic addressing has the following structure - ValidLabel[MinNum-MaxNum]
 * The following rules are being checked -
 *  1. A valid label exists before '['
 *  2. MinNum exist and is in the allowed range
 *  3. MaxNum exist and is in the allowed range
 *  4. MinNum should be smaller the MaxNum
 *  5. the diff between MaxNum and MinNum should be in the allowed range
 * @param operandStr - String to check for dynamic addressing type
 * @param label - pointer to label to set at the end of the process, if all rules met
 * @param minNum - pointer to minNum to set at the end of the process, if all rules met
 * @param maxNum - pointer to maxNum to set at the end of the process, if all rules met
 * @return ErrMsg - char* - NULL if no issues, errMsg otherwise
 */
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

    operandStrSize = (int)strlen(operandStr);       /* length of operantStr */
    openBracketPosStr = strchr(operandStr, '[');    /* locate '[' string start in operandStr*/
    closeBracketPosStr = strchr(operandStr, ']');   /* locate ']' string start in operandStr */
    dashPosStr = strchr(operandStr, '-');           /* locate '-' string start in operandStr */

    openBracketPos = (int) (openBracketPosStr - operandStr);    /* locate '[' position in operandStr*/
    closeBracketPos = (int) (closeBracketPosStr - operandStr);  /* locate ']' position in operandStr */
    dashPos = (int) (dashPosStr - operandStr);                  /* locate '-' position in operandStr */

    if (closeBracketPos > dashPos && dashPos > openBracketPos &&    /* Check '[' then '-' then ']' */
        openBracketPos != 0 &&                                      /* Has label instead of [ */
        closeBracketPos == operandStrSize - 1) {                    /* ']' is at the end */

        labelTemp = getNewSubString(operandStr, openBracketPos);    /* get string before '[' as a label candidate */
        errStr = isLabelValid(labelTemp);                           /* check if string is a valid label */
        if (errStr != NULL) {                                       /* If string is not valid label, return error */
            return errMessage(errStr, labelTemp);
        }
        minNumStr = getNewStrBetweenTwoChars(operandStr, '[', '-', false, false);       /* get number between '[' and '-' */
        if (minNumStr != NULL) {                                                        /* check number is not null */
            minNumTemp = getIntFromString(minNumStr, MAX_CMD_NUMBER);                   /* cast char* to a number */
            if (minNumTemp == INVALID_NUM_TOKEN || minNumTemp > MAX_DYNAMIC_OPERAND) {  /* check casting was ok and number does not exceed the allowed number for dynamic operand */
                errStr = errMessage(ERR_NUM_OUT_OF_BOUNDS, minNumStr);                  /* throw an error - out of bounds */
                free(minNumStr);
                return errStr;
            }
            free(minNumStr);
        }
        else {
            return errMessage(ERR_BAD_DYNAMIC_ADDRESSING, operandStr); /* String is empty - throw error */
        }
        maxNumStr = getNewStrBetweenTwoChars(operandStr, '-', ']', false, false);       /* get number between '-' and ']' */
        if (maxNumStr != NULL) {                                                        /* check number is not null */
            maxNumTemp = getIntFromString(maxNumStr, MAX_CMD_NUMBER);                   /* cast char* to a number */
            if (maxNumTemp == INVALID_NUM_TOKEN || maxNumTemp > MAX_DYNAMIC_OPERAND) {  /* check casting was ok and number does not exceed the allowed number for dynamic operand */
                errStr = errMessage(ERR_NUM_OUT_OF_BOUNDS, maxNumStr);                  /* throw an error - out of bounds */
                free(minNumStr);
                return errStr;
            }
            free(maxNumStr);
        }
        else {
            return errMessage(ERR_BAD_DYNAMIC_ADDRESSING, operandStr); /* String is empty - throw error */
        }

        if (minNumTemp > maxNumTemp) { /* check if left number is bigger than left number - throw error if so */
            return copyString(ERR_LEFT_RIGHT_BOUNDS);
        }
        if (maxNumTemp - minNumTemp >= MAX_CMD_NUMBER) { /* check if the difference between two numbers excceed the allowed threshold - throw error if so */
            return copyString(ERR_LEFT_RIGHT_RANGE);
        }

        *label = labelTemp;     /* set the label */
        *minNum = minNumTemp;   /* set min number */
        *maxNum = maxNumTemp;   /* set max number */
        return NULL;            /* return no error message */
    }
    return errMessage(ERR_BAD_DYNAMIC_ADDRESSING, operandStr);
}

/**
 * gets an operand string and set operand (Operand type) with the appropriate data
 * Appropriate data for each addressing type -
 *  1. Direct Adressing -       operand->addressingType = NUMBER
 *                              operand->value = number after #
 *  2. Register Addressing -    operand->addressingType = REGISTER
 *                              operand->registerNum = number after 'r'
 *  3. Dynamic -                operand->addressingType = DYNAMIC
 *                              operand->label = label
 *                              operand->minNum = minNum
 *                              operand->maxNum = maxNum
 *  4. Direct -
 *
 * @param operandStr - Holds one operand string
 * @param operand    - Struct that holds all operand's metadata
 * @return - Sets operand values according to the appropriate operand type, throws error when something goes wrong
 */
char* getOperand(char* operandStr, Operand* operand) {
    int num;
    char* label;

    char* openBracketPosStr;
    char* closeBracketPosStr;
    char* dashPosStr;
    int minNum;
    int maxNum;
    char* errStr;

    openBracketPosStr = strchr(operandStr, '['); /* Find pointer of '[' string start */
    closeBracketPosStr = strchr(operandStr, ']'); /* Find pointer of ']' string start */
    dashPosStr = strchr(operandStr, '-'); /* Find pointer of '-' string start */

    /* check direct addressing */
    if (operandStr[0] == '#') { /* if operand string starts with # */
        num = getIntFromString(++operandStr, MAX_CMD_NUMBER); /* get the number after # and check it does not exceed boundaries */
        if (num != INVALID_NUM_TOKEN) { /* if number does not exceeds boundaries, set direct addressing operand values */
            operand->addressingType = NUMBER;  /* set addressing type as NUMBER (Direct Addressing) */
            operand->value = num;              /* set value of operand as number after # */
            return NULL;
        }
        else {
            return errMessage(ERR_INVALID_NUMBER, operandStr); /* throw error if number exceeds boundaries */
        }
    }
    /*  Register Addressing */
    else if (operandStr[0] == 'r'                           /* operand string starts with 'r' */
             && strlen(operandStr) == MAX_REGISTER_CHARS    /* length of operand string is 2 */
             && operandStr[1] >= MIN_REGISTER_NUM           /* second char is equal bigger than 0 */
             && operandStr[1] <= MAX_REGISTER_NUM) {        /* second char is equal smaller than 7 */
            operand->addressingType = REGISTER;              /* set addressing type as register */
            operand->registerNum = (int)operandStr[1] - '0'; /* set register value to the one found */
            return NULL;
    }
    /* Dinami Yashir */
    else if (openBracketPosStr != NULL && closeBracketPosStr != NULL && dashPosStr != NULL) { /* Check we should be in Dinami Yashir - has '[', '-' and ']' in string */
        errStr = checkDynamicAddressing(operandStr, &label, &minNum, &maxNum); /* Set label, minNum and maxNum with the appropriate values, get an error message if string is not in the correct structure of Dynamic addressing string  */
        if (errStr == NULL) {                   /* If no error */
            operand->addressingType = DYNAMIC;  /* set addressing type as Dynamic addressing */
            operand->label = label;             /* set label as label found */
            operand->minNum = minNum;           /* set min number for dynamic addressing as minNum */
            operand->maxNum = maxNum;           /* set max number for dynamic addressing as maxNum */
            return NULL;
        }
        else {
            return errStr; /* Throw error */
        }
    }
    /* Yashir */
    else if (isLabelValid(operandStr) == NULL) { /* check if we are in direct addressing by checking if label is valid */
        operand->addressingType = DIRECT;           /* set addressing type as Direct */
        operand->label = copyString(operandStr);    /* set label for direct as the label found */
        return NULL;
    }
    return errMessage(ERR_UNKNOWN_ADDRESSING, operandStr); /* throw error - addressing does not correspond to any known addressing type */
}

/**
 * Gets remaining string after the command (mov, cmp..), expecting 2 operands,
 * parses it according to the operands in it and sets both operands of the line
 * @param rawOperandsString - String after command string
 * @param parsedLine - FileLine struct related to the line working on now
 * @param checkAddressing - Set to True when command has a restriction on destination opernd (all cmd except for cmp and prn)
 * @return NULL if everything ok, error string if there was any issue
 */
char* checkTwoOperands(char* rawOperandsString, FileLine* parsedLine, bool checkAddressing){
    char* firstRawOperand;
    char* secondRawOperand;
    char* errString;


    if (rawOperandsString == NULL) { /* check at least one operand */
        return errMessage(ERR_TWO_OP_GOT_NONE, parsedLine->action);
    }
    if (getNumCommas(rawOperandsString) != 1) { /* check that there is only one comma (,) in command */
        return errMessage(ERR_INVALID_COMMAND_FORMAT, rawOperandsString);
    }
    firstRawOperand = strtok(rawOperandsString, " ,\t"); /* get first operand - split by comma and/or space */
    secondRawOperand = strtok(NULL, " ,\t"); /* get second operand - split by comma and/or space */

    if (secondRawOperand == NULL){ /* check at least two operand */
        return errMessage(ERR_TWO_OP_GOT_ONE, rawOperandsString);
    }
    if (strtok(NULL, " ,\t") != NULL){ /* check no more than 2 operands */
        return errMessage(ERR_TWO_OP_GOT_MORE, rawOperandsString);
    }

    parsedLine->numOfCommandOprands = 2; /* set line number of operands = 2 */

    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));
    errString = getOperand(firstRawOperand, parsedLine->firstOperValue); /* Parse first operand and set it as parsedline-> firstOperValue */
    if (errString != NULL) return errString; /* Throw error if parsing failed */

    parsedLine->secondOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->secondOperValue, 0, sizeof(Operand));
    errString = getOperand(secondRawOperand, parsedLine->secondOperValue); /* Parse second operand and set it as parsedline-> secondOperValue  */
    if (errString != NULL) return errString; /* Throw error if parsing failed */

    if (checkAddressing && (parsedLine->secondOperValue->addressingType == NUMBER || /* If checkAdressing true (any cmd that is not cmp or prn) ,check if the destination operand is 0,2 and if so throw and error */
            parsedLine->secondOperValue->addressingType == DYNAMIC))
        return errMessage(ERR_ILLEGAL_DEST_ADDRESSING, parsedLine->action);

    return NULL;
}

/**
 * Gets remaining string after the command (mov, cmp..), expecting 1 operand,
 * parses it according to the operand in it and sets operand in line
 * @param rawOperandsString - String after command string
 * @param parsedLine - FileLine struct related to the line working on now
 * @param checkAddressing - Set to True when command has a restriction on destination opernd (all cmd except for cmp and prn)
 * @return NULL if everything ok, error string if there was any issue
 */
char* checkOneOperand(char* rawOperandsString, FileLine* parsedLine, bool checkAddressing){
    char* firstRawOperand;
    char* errString;

    if (rawOperandsString == NULL) { /* Check at least one operand */
        return errMessage(ERR_ONE_OP_GOT_NONE, parsedLine->action);
    }
    if (getNumCommas(rawOperandsString) != 0) { /* Check string formatting (no commas) */
        return errMessage(ERR_INVALID_COMMAND_FORMAT, rawOperandsString);
    }

    firstRawOperand = strtok(rawOperandsString, " ,\t"); /* get first operand - split by comma and/or space */
    if (strtok(NULL, " ,\t") != NULL){ /* if rest of string is not empty, throw error */
        return errMessage(ERR_ONE_OP_GOT_MORE, rawOperandsString);
    }
    else if (firstRawOperand == NULL){ /* if operand 1 is empty, throw error */
        return errMessage(ERR_ONE_OP_GOT_NONE, parsedLine->action);
    }

    parsedLine->numOfCommandOprands = 1; /* set line number of operands = 1 */
    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));
    errString = getOperand(firstRawOperand, parsedLine->firstOperValue); /* Parse operand string and set parsedLine->firstOperValue with it if no error */
    if (errString != NULL) return errString; /* If error in parsing, throw error */

    if (checkAddressing && (parsedLine->firstOperValue->addressingType == NUMBER || /* If checkAdressing true (any cmd that is not cmp or prn) ,check if the destination operand is 0,2 and if so throw and error */
            parsedLine->firstOperValue->addressingType == DYNAMIC))
       return errMessage(ERR_ILLEGAL_DEST_ADDRESSING, parsedLine->action);

    return NULL;
}

/**
 * Gets operands string, check that it is empty
 * @param rawOperandsString - Rest of string after command
 * @param parsedLine - FileLine struct to update num of operands in
 * @return NULL if no error, error message otherwise
 */
char* checkNoOperand(char* rawOperandsString, FileLine* parsedLine){
    char* firstRawOperand;
    firstRawOperand = strtok(rawOperandsString, " \t");
    if (firstRawOperand != NULL){ /* Operand 1 is not empty */
        return errMessage(ERR_NO_OP_GOT_MORE, rawOperandsString);
    }
    parsedLine->numOfCommandOprands = 0; /* set line number of operands = 0 */
    return NULL;
}

/**
 * gets string after .data command, check for validity and store in parsedLine
 * @param rawOperandsString - Rest of string after command
 * @param parsedLine - FileLine struct to update num of operands in
 * @return NULL if no error, error message otherwise
 */
char* checkDataOperand(char* rawOperandsString, FileLine* parsedLine) {
    int dataSize = 1;
    int i;
    int* data;
    char* numString;
    int numberInt;

    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));

    if (rawOperandsString == NULL) { /* Check the existence of data, if not exist, throw error */
        return copyString(ERR_DATA_INVALID);
    }

    dataSize += getNumCommas(rawOperandsString); /* Amount of data objects is number of commas plus 1 (initiated with 1) */
    data = (int*)malloc(dataSize * sizeof(int));
    memset(data, 0, dataSize * sizeof(int));
    numString = strtok(rawOperandsString, " ,\t\n\r"); /* get first data object - number */
    if (numString == NULL) { /* if first data object is empty, throw error */
        free(data);
        return copyString(ERR_DATA_INVALID);
    }
    for (i=0; i < dataSize; i++){ /* for each of the data objects, parse the data, check for boundaries and save it in an array */
        numberInt = getIntFromString(numString, MAX_DATA_NUMBER); /* get integer from string */
        if (numberInt != INVALID_NUM_TOKEN){ /* if no error parsing string */
            data[i] = numberInt; /* save number in data array */
            numString = strtok(NULL, " ,\t\n\r"); /* get next data object */
            if (numString == NULL && i < dataSize - 1) { /* check next iteration (except for last) has valid data object, throw error if not */
                free(data);
                return copyString(ERR_DATA_INVALID);
            }
        }
        else { /* Throw error if data string is not valid as a number or out of boundaries */
            free(data);
            return errMessage(ERR_DATA_OUT_OF_BOUNDS, numString);
        }
    }
    if (numString != NULL){ /* if there are more data objects then number of commas + 1, throw error */
        free(data);
        return copyString(ERR_DATA_INVALID);
    }
    parsedLine->firstOperValue->data = data; /* set data array under operand in parsedLine */
    parsedLine->firstOperValue->dataSize = dataSize; /* set data array size under operand in parsedLine */
    return NULL;
}

/**
 * gets string after .string command, check for validity and store in parsedLine
 * @param rawOperandString - Rest of string after command
 * @param parsedLine - FileLine struct to update num of operands in
 * @return NULL if no error, error message otherwise
 */
char* checkStringOperand(char* rawOperandString, FileLine* parsedLine) {
    char* string;

    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));

    if (rawOperandString == NULL) { /* if string is empty, throw error */
        return copyString(ERR_DATA_INVALID);
    }

    string = getNewStrBetweenTwoChars(rawOperandString, '"', '"', true, true); /* get string between double quotes, throw error if there are characters before first double quotes or after second double quotes */
    if (string == NULL) /* if any error parsing string, throw error */
        return errMessage(ERR_STRING_INVALID, rawOperandString);

    parsedLine->firstOperValue->string = string; /* save string in parsedLine, under first operand */
    return NULL;
}

/**
 * gets string after .extern/.entry command, check for validity and store in parsedLine
 * @param rawOperandString - Rest of string after command
 * @param parsedLine - FileLine struct to update num of operands in
 * @return NULL if no error, error message otherwise
 */
char* checkExternOrEntryOperand(char* rawOperandString, FileLine* parsedLine) {
    char* label;
    char* errStr;

    parsedLine->firstOperValue = (Operand*) malloc(sizeof(Operand));
    memset(parsedLine->firstOperValue, 0, sizeof(Operand));

    if (rawOperandString == NULL) { /* check string is not empty, if so, throw error */
        return errMessage(ERR_ONE_OP_GOT_NONE, parsedLine->action);
    }

    label = strtok(rawOperandString, " \t\n\r");
    errStr = isLabelValid(label); /* check that string has a valid label */
    if (errStr == NULL){ /* if label is not valid, throw error */
        parsedLine->firstOperValue->entryOrExtern = copyString(label); /* set entry/extern label under first operand in parsedLine */
        return NULL;
    }
    return errMessage(errStr, label);
}

/**
 * Check that action string is valid, has the right amount of operands after and checks
 * @param rawOperandsString
 * @param parsedLine - FileLine struct to update num of operands in
 * @return NULL if line is ok, else error message
 */
char* validateActionAndOperands(char* rawOperandsString, FileLine* parsedLine) {
    char* errStr = NULL;
    char* action = parsedLine->action;
    if (strcmp(action, "mov") == 0) { /* if action is mov, set action and parse the two operands */
        parsedLine->actionType = MOV;
        errStr = checkTwoOperands(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "cmp") == 0){ /* if action is cmp, set action and parse the two operands */
        parsedLine->actionType = CMP;
        errStr = checkTwoOperands(rawOperandsString, parsedLine, false); /* false as no need to check destination operand addressing */
    }
    else if (strcmp(action, "add") == 0){ /* if action is add, set action and parse the two operands */
        parsedLine->actionType = ADD;
        errStr = checkTwoOperands(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "sub") == 0){ /* if action is sub, set action and parse the two operands */
        parsedLine->actionType = SUB;
        errStr = checkTwoOperands(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "lea") == 0){ /* if action is lea, set action, parse the two operands and check that source operand is Direct */
        parsedLine->actionType = LEA;
        errStr = checkTwoOperands(rawOperandsString, parsedLine, true);
        if (errStr == NULL && parsedLine->firstOperValue != NULL && /* check first operand exist and is Direct */
                parsedLine->firstOperValue->addressingType != DIRECT)
            errStr = copyString(ERR_LEA_SOURCE_ADDRESSING);
    }
    else if (strcmp(action, "clr") == 0){ /* if action is clr, set action and parse the operand */
        parsedLine->actionType = CLR;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "not") == 0){ /* if action is not, set action and parse the operand */
        parsedLine->actionType = NOT;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "inc") == 0){ /* if action is inc, set action and parse the operand */
        parsedLine->actionType = INC;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "dec") == 0){ /* if action is dec, set action and parse the operand */
        parsedLine->actionType = DEC;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "jmp") == 0){ /* if action is jmp, set action and parse the operand */
        parsedLine->actionType = JMP;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "bne") == 0){ /* if action is bne, set action and parse the operand */
        parsedLine->actionType = BNE;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "red") == 0){ /* if action is red, set action and parse the operand */
        parsedLine->actionType = RED;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "jsr") == 0){ /* if action is jsr, set action and parse the operand */
        parsedLine->actionType = JSR;
        errStr = checkOneOperand(rawOperandsString, parsedLine, true);
    }
    else if (strcmp(action, "prn") == 0){ /* if action is prn, set action and parse the operand */
        parsedLine->actionType = PRN;
        errStr = checkOneOperand(rawOperandsString, parsedLine, false); /* false as no need to check destination operand addressing */
    }
    else if (strcmp(action, "rts") == 0){ /* if action is rts, set action and check no operand */
        parsedLine->actionType = RTS;
        checkNoOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, "stop") == 0){ /* if action is stop, set action and check no operand */
        parsedLine->actionType = STOP;
        errStr = checkNoOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, ".data") == 0){ /* if action is .data, set action and parse data */
        parsedLine->actionType = DATA;
        errStr = checkDataOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, ".string") == 0){ /* if action is .string, set action and parse string */
        parsedLine->actionType = STRING;
        errStr = checkStringOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, ".entry") == 0){ /* if action is .entry, set action and save label */
        parsedLine->actionType = ENTRY;
        errStr = checkExternOrEntryOperand(rawOperandsString, parsedLine);
    }
    else if (strcmp(action, ".extern") == 0){ /* if action is .extern, set action and save label */
        parsedLine->actionType = EXTERN;
        errStr = checkExternOrEntryOperand(rawOperandsString, parsedLine);
    }
    else { /* Command is not known, throw error */
        parsedLine->actionType = UNKNOWN;
        errStr = errMessage(ERR_UNKNOWN_CMD, action);
    }
    return errStr;
}

/**
 * check line for label, command and operands validity
 * @param parsedLine - FileLine struct to update num of operands in
 * @return NULL if line is ok, else error message
 */
char* lineValidator(FileLine* parsedLine) {
    char* string;
    char* parsedLabel;
    int strSize;
    char* errStr;

    char* lineToCheck = copyString(parsedLine->originalLine);
    string = strtok(lineToCheck, " \t\n\r"); /* Get first string in line */

    /* Handle empty lines */
    if (string == NULL){
        parsedLine->isEmptyOrComment = true;
        free(lineToCheck);
        return NULL;

    }/* Handle Comment Lines - if line starts with ';' */
    else if (strlen(string) >= 1 && string[0] == ';'){
        parsedLine->isEmptyOrComment = true;
        free(lineToCheck);
        return NULL;
    }

    /* Handle labels */
    strSize = (int)strlen(string);
    if (string[strSize - 1] == ':') { /* Label Found if last char is ':' */
        if (string - lineToCheck != 0){ /* check that label starts in first line column */
            errStr = errMessage(ERR_INVAILD_LABEL_START, string);
            free(lineToCheck);
            return errStr;
        }
        parsedLabel = getNewSubString(string, strSize - 1); /* get string without ':' */
        errStr = isLabelValid(parsedLabel); /* check label is valid */
        if (errStr == NULL){ /* if label is valid, set it as line label */
            parsedLine->label = parsedLabel; /* set line label */
        }
        else {  /* if label is not valid, throw error */
            free(lineToCheck);
            errStr = errMessage(errStr, parsedLabel);
            free(parsedLabel);
            return errStr;
        }
        string = strtok(NULL, " \t\n\r"); /* Get action string */
    }

    parsedLine->action = copyString(string);      /* get the action */
    string = strtok(NULL, "\n\r"); /*  get remaining of line */
    errStr = validateActionAndOperands(string, parsedLine); /* check operands by the command recieved */
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
    if (fr == NULL) { /* if file does not exist, throw error and return Fail */
        printInternalError(ERR_FILE_NOT_FOUND, filename);
        return Fail;
    }
    fileContent->filename = copyString(filename); /* save file name on line */
    while(fgets(line, sizeof(line), fr) != NULL)   /* get a next line, done if NULL */
    {
        if(DEBUG) printf("Line is -->  %s", line);

        parsedLine = &fileContent->line[arrayIndex]; /* save line object in file content lines array */
        memset(parsedLine, 0, sizeof(FileLine));

        parsedLine->lineNumber = lineCounter; /* set line number of line in line object */
        parsedLine->originalLine = copyString(line); /* set original line string in line object */
        errString = lineValidator(parsedLine); /* verify line label, command and operands validity */
        if (errString != NULL) { /* if line not valid, return fail and write the error */
            fileStatus = Fail;
            printSyntaxError(errString, filename, lineCounter);
        }
        if (parsedLine->isEmptyOrComment == false) { /* only if line is not empty or comment, increase line counter for file content */
            arrayIndex++;
        }
        lineCounter++;
    }
    fileContent->size = arrayIndex; /* save amount of lines that are not comment or empty */
    fclose(fr);  /* close the file prior to exiting the routine */
    return fileStatus;
}

/**
 * Allocate space for file lines
 * @param fileContent - pointer to filecontent struct
 * @return Pass if no issues allocating memory, Fail otherwise
 */
Status initFileContent(FileContent** fileContent) {
    *fileContent = (FileContent*)malloc(sizeof(FileContent));
    memset(*fileContent, 0, sizeof(FileContent));
    if (*fileContent == NULL) /* if memory allocation for file content failed, return Fail */
        return Fail;

    (*fileContent)->line = (FileLine*)malloc(sizeof(FileLine)*MAX_FILE_LINES);
    if ((*fileContent)->line == NULL) /* if memory allocation for line array failed, return Fail */
        return Fail;
    return Pass;
}

/**
 * gets a pointer to filecontent and cleans its memory
 * @param fileContent - Object to clean memory for
 */
void freeFileContent(FileContent** fileContent) {
    int i;
    if ((*fileContent)->size > 0){
        if(DEBUG) printf("Clears file line\n");
        for (i = 0; i < (*fileContent)->size; ++i) { /* clears memory of all lines in file content line array */
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
    free((*fileContent)->line); /* after cleaning the inside of the fileContent object, we clear its memory too */
    if ((*fileContent)->filename != NULL)
        free((*fileContent)->filename);
    free(*fileContent);
}

