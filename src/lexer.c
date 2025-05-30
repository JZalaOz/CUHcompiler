#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "common.h"
#include "lexer.h"
#include "errors.h"

#define LEXER_ERROR_NEARBY_CHARS_SHOW 15

struct LexerState {
    LexerResult result;
    int readPos; // Starts from 0 upto rawC
    int rawC; // the size of rawV
    char* rawV;

    int line;
    int lineIndex;
};

char* getStrFromTo(struct LexerState* state, int index, int length) {
    char* value = malloc(length + 1);
    checkIfAllocationFailed(value);

    for (int i = 0; i < length; i++) {
        value[i] = state->rawV[index + i];
    }

    // Set the last char to null for end of string
    value[length] = '\0';
    return value;
}

int getRemainingChars(struct LexerState* state) {
    return state->rawC - state->readPos;
}

bool doStrMatch(char* str1, char* str2) {
    return strcmp(str1, str2) == 0;
}

int digitCount(const int value) {
    int i = value;
    int count = 0;

    while (i > 0) {
        i /= 10;
        count++;
    }

    return count;
}

void throwLexerError(struct LexerState* state, char* message) {
    textColorStdErr(ATTRIBUTE_BRIGHT, COLOR_RED);
    fprintf(stderr, "\nLexical Error: %s\n", message);

    int lineNunberDigits = digitCount(state->line);
    char lineNumber[lineNunberDigits + 1];
    sprintf(lineNumber, "%d\0", state->line);
    
    // Print line number
    textColorStdErr(ATTRIBUTE_BRIGHT, COLOR_WHITE);
    printf("\n%s", &lineNumber);

    // Print the line
    int left = maxInt(0, state->lineIndex - LEXER_ERROR_NEARBY_CHARS_SHOW);
    int right = minInt(state->rawC, state->lineIndex + LEXER_ERROR_NEARBY_CHARS_SHOW);

    int lineSize = 0;
    char* line = NULL;

    // add all of the chars to the left including the error location char
    // but reading from right to left so we can stop when theres a new line
    for (int i = state->readPos; i > left; i--) {
        char currentChar = *getStrFromTo(state, i, 1);

        if (currentChar == '\n') {
            break;
        }

        lineSize++;
        char* newLine = malloc(sizeof(char) * (lineSize + 1));
        checkIfAllocationFailed(newLine);
        newLine[0] = currentChar;

        if (line == NULL) {
            newLine[1] = '\0';
            line = newLine;
        } else {
            strcpy(newLine + 1, line);
            free(line);
            line = newLine;
        }
    }

    if (line == NULL) {
        line = malloc(0);
    }

    // Add all the remaining chars to the right of the error char
    // we stop when we reach cap or new line
    for (int i = 0; i < LEXER_ERROR_NEARBY_CHARS_SHOW; i++) {
        int charIndex = state->readPos + i + 1;
        if (charIndex > state->rawC) break;

        char currentChar = *getStrFromTo(state, charIndex, 1);

        if (currentChar == '\n') {
            break;
        }

        lineSize++;
        char* newLine = malloc(sizeof(char) * (lineSize + 1));
        checkIfAllocationFailed(newLine);

        strcpy(newLine, line);
        newLine[lineSize - 1] = currentChar;
        free(line);
        line = newLine;
    }

    textColorStdErr(ATTRIBUTE_BRIGHT, COLOR_BLACK);
    printf("  %s\n  ", line);
    
    // Print cursor
    for (int i = (strlen(line) / 2) + lineNunberDigits + 1; i > 0; i--)
        printf(" ");
    
    textColorStdErr(ATTRIBUTE_BRIGHT, COLOR_GREEN);
    printf("^\n\n");

    resetTextColorStdErr();
    exit(1);
}

void pushToken(struct LexerState* state, LexerTokenType type, char* value) {
    LexerResult* result = &state->result;

    LexerToken token;
    token.type = type;
    token.value = value;

    result->tokenV[result->tokenC] = token;
    result->tokenC += 1;

    LexerToken* temp = realloc(result->tokenV, (result->tokenC + 1) * sizeof(LexerResult));
    checkIfAllocationFailed(temp);
    result->tokenV = temp;
}

void nextReadPos(struct LexerState* state) {
    state->readPos++;
    char currentChar = *getStrFromTo(state, state->readPos, 1);

    if (currentChar == '\n') {
        state->line++;
        state->lineIndex = 1;
    } else {
        state->lineIndex++;
    }
}

void pushBasicToken(struct LexerState* state, LexerTokenType type) {
    pushToken(state, type, NULL);
    nextReadPos(state);
}

bool isWhiteSpace(char* value, bool isString) {
    int index = 0;

    while (true) {
        char current = value[index];

        if (current == ' ' || current == '\t' || current == '\r' || current == '\n') {
            if (!isString) {
                return true;
            } else {
                index++;
                continue;
            }
        }

        if (current == '\0' && isString) {
            return true;
        }

        return false;
    }

    return false;
}

bool processPunctuation(struct LexerState* state) {
    char currentChar = *getStrFromTo(state, state->readPos, 1);
    LexerTokenType token = TOKEN_UNKNOWN;

    switch (currentChar) {
        case ';':
            token = TOKEN_SEMI_COLON;
            break;
        case ',':
            token = TOKEN_COMMA;
            break;
        case '(':
            token = TOKEN_OPEN_PARENTHESES;
            break;
        case ')':
            token = TOKEN_CLOSE_PARENTHESES;
            break;
        case '{':
            token = TOKEN_OPEN_BRACES;
            break;
        case '}':
            token = TOKEN_CLOSE_BRACES;
            break;
        case '[':
            token = TOKEN_OPEN_BRACKET;
            break;
        case ']':
            token = TOKEN_CLOSE_BRACKET;
            break;
        default:
            return false;
    }

    pushBasicToken(state, token);
    return true;
}

bool processOperations(struct LexerState* state) {
    char currentChar = *getStrFromTo(state, state->readPos, 1);
    int remainingChars = getRemainingChars(state);

    LexerTokenType token = TOKEN_UNKNOWN;
    char* value = NULL;

    switch (currentChar) {
        case '+':
            token = TOKEN_ADD;
            break;
        case '-':
            token = TOKEN_SUBTRACT;
            break;
        case '*':
            token = TOKEN_MULIPLY;
            break;
        case '/':
            token = TOKEN_DIVIDE;
            break;
        case '%':
            token = TOKEN_MODULO;
            break;
        case '!':
            if (remainingChars > 1 && state->rawV[state->readPos + 1] == '=') {
                pushBasicToken(state, TOKEN_NOT_EQUAL);
                nextReadPos(state);
            } else {
                state->readPos++; // Only doing it here cause throwing an error
                char message[] = "Expeceted !=, got !";
                strcat(message, getStrFromTo(state, state->readPos, 1));
                throwLexerError(state, message);
            }
            break;
        case '=':
            if (remainingChars > 1 && state->rawV[state->readPos + 1] == '=') {
                pushBasicToken(state, TOKEN_EQUAL);
                nextReadPos(state);
            } else {
                pushBasicToken(state, TOKEN_ASSIGN);
            }
            break;
        default:
            return false;
    }

    if (token != TOKEN_UNKNOWN) {
        pushToken(state, token, value);
        if (value != NULL) {
            for (int i = 0; i < strlen(value); i++) {
                nextReadPos(state);
            }
        } else {
            nextReadPos(state);
        }
    }

    return true;
}

bool processSpecialKeywords(struct LexerState* state, char* value) {
    LexerTokenType keywordToken = TOKEN_UNKNOWN;

    if (doStrMatch(value, "val")) {
        keywordToken = TOKEN_VAL;
    } else if (doStrMatch(value, "var")) {
        keywordToken = TOKEN_VAR;
    } else if (doStrMatch(value, "if")) {
        keywordToken = TOKEN_IF;
    } else {
        return false;
    }

    pushBasicToken(state, keywordToken);
    return true;
}

bool processNumbers(struct LexerState* state, int valueLength, char* value) {
    bool isPastDecimal = false;

    LexerTokenType tokenType = TOKEN_UNKNOWN;

    bool hasLeft = false;
    bool hasRight = false;
    int index = 0;

    while (index < valueLength) {
        char currentChar = value[index];
        bool isDigit = (currentChar >= '0' && currentChar <= '9');

        if (currentChar == '.' && !isPastDecimal) {
            tokenType = TOKEN_DOUBLE;
            isPastDecimal = true;
            index++;
            continue;
        }

        if (isDigit) {
            if (tokenType == TOKEN_UNKNOWN) {
                tokenType = TOKEN_INTEGER;
            }

            if (isPastDecimal) {
                hasRight = true;
            } else {
                hasLeft = true;
            }

            index++;
            continue;
        }

        if (currentChar == 'f' && (hasLeft || hasRight)) {
            tokenType = TOKEN_FLOAT;
            index++;
            break;
        }

        break;
    }

    switch (tokenType) {
        case TOKEN_INTEGER:
            pushToken(state, TOKEN_INTEGER, value);
            break;
        case TOKEN_DOUBLE:
            pushToken(state, TOKEN_DOUBLE, value);
            break;
        case TOKEN_FLOAT:
            pushToken(state, TOKEN_FLOAT, value);
        default:
            return false;
    }

    return true;
}

bool processIdentifier(struct LexerState* state, int valueLength, char* value) {
    int index = 0;

    while (index < valueLength) {
        char currentChar = value[index];

        bool isLowerCase = (currentChar >= 'a' && currentChar <= 'z');
        bool isUpperCase = (currentChar >= 'A' && currentChar <= 'Z');
        bool isDigit = (currentChar >= '0' && currentChar <= '9');

        if (isDigit && index == 0) return false;
        if (!isLowerCase && !isUpperCase && (index == 0 || !isDigit) && currentChar != '_') {
            return false;
        }

        index++;
    }

    pushToken(state, TOKEN_IDENTIFIER, value);
    return true;
}

bool processWordBasedTokens(struct LexerState* state) {
    int valueLen = 0;

    while (getRemainingChars(state) > 0) {
        char currentChar = *getStrFromTo(state, state->readPos, 1);

        bool isLowerCase = (currentChar >= 'a' && currentChar <= 'z');
        bool isUpperCase = (currentChar >= 'A' && currentChar <= 'Z');
        bool isDigit = (currentChar >= '0' && currentChar <= '9');

        if (!isLowerCase && !isUpperCase && currentChar != '_' && currentChar != '.' && !isDigit) {
            break;
        }

        valueLen++;
        nextReadPos(state);
    }

    if (valueLen == 0) {
        return false;
    }

    char* value = getStrFromTo(state, state->readPos - valueLen, valueLen);

    if (doStrMatch(value, "true") || doStrMatch(value, "false")) {
        pushToken(state, TOKEN_BOOLEAN, value);
    } else if (processSpecialKeywords(state, value)) {
    } else if (processIdentifier(state, valueLen, value)) {
    } else if (processNumbers(state, valueLen, value)) {
    } else {
        return false;
    }

    return true;
}

bool processStringToken(struct LexerState* state) {
    if (((char) *getStrFromTo(state, state->readPos, 1)) != "\"") {
        return false;
    }

    int length = 0;

    while (getRemainingChars(state) > 0) {
        char currentChar = *getStrFromTo(state, state->readPos, 1);

        if (currentChar != "\"") {
            length++;
            nextReadPos(state);
            continue;
        }

        if (getRemainingChars(state) == 0) {
            throwLexerError(state, "String not ended, expected \" at some point");
        }

        break;
    }

    if (length == 0) {
        pushToken(state, TOKEN_STRING, "");
        return true;
    }

    char* value = getStrFromTo(state, state->readPos - length, length);

    pushToken(state, TOKEN_STRING, value);
    return true;
}

LexerResult tokenize(int rawCodeC, char* rawCodeV) {
    printf("Tokenizing the raw code...\n");

    struct LexerState state;

    state.result.tokenC = 0;
    state.result.tokenV = malloc(sizeof(LexerToken));
    checkIfAllocationFailed(state.result.tokenV);
    state.readPos = 0;
    state.rawC = rawCodeC;
    state.rawV = rawCodeV;
    state.line = 1;
    state.lineIndex = 1;

    while (getRemainingChars(&state) > 0) {
        char currentChar = *getStrFromTo(&state, state.readPos, 1);

        if (processPunctuation(&state)) {
            continue;
        }

        if (processOperations(&state)) {
            continue;
        }

        if (processWordBasedTokens(&state)) {
            continue;
        }

        if (processStringToken(&state)) {
            continue;
        }

        if (isWhiteSpace(&currentChar, false)) {
            nextReadPos(&state);
            continue;
        }

        if (getRemainingChars(&state) > 0) {
         throwLexerError(&state, "HUH??! WTF");
        }
    }

    pushBasicToken(&state, TOKEN_EOF);
    return state.result;
}