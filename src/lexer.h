#ifndef LEXER_H
#define LEXER_H

typedef enum LexerTokenType LexerTokenType;
enum LexerTokenType {
    TOKEN_IDENTIFIER, // name of a variable or function
    // Keywords
    TOKEN_VAL, 
    TOKEN_VAR, 
    TOKEN_IF,
    // Punctuation
    TOKEN_SEMI_COLON, // ;
    TOKEN_COMMA, // ,
    TOKEN_OPEN_PARENTHESES, // (
    TOKEN_CLOSE_PARENTHESES, // )
    TOKEN_OPEN_BRACES, // {
    TOKEN_CLOSE_BRACES, // }
    TOKEN_OPEN_BRACKET, // [
    TOKEN_CLOSE_BRACKET, // ]
    // Literals
    TOKEN_BOOLEAN, // true or false
    TOKEN_INTEGER, // c signed integer
    TOKEN_DOUBLE, // c double
    TOKEN_FLOAT, // IEEE-754 float number
    // Operations
    TOKEN_ADD, // +
    TOKEN_SUBTRACT, // -
    TOKEN_MULIPLY, // *
    TOKEN_DIVIDE, // /
    TOKEN_MODULO, // %
    TOKEN_ASSIGN, // =
    TOKEN_NOT_EQUAL, // !=
    TOKEN_EQUAL, // ==
    // Other
    TOKEN_EOF, // end of file
    TOKEN_UNKNOWN,
};

typedef struct {
    LexerTokenType type;
    char* value;
} LexerToken;

typedef struct {
    int tokenC;
    LexerToken* tokenV;
} LexerResult;

LexerResult tokenize(int rawC, char* rawV);

#endif