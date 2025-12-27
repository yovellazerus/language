#ifndef LEXER_H
#define LEXER_H

#include "common.h"
#include "error.h"

typedef enum {
    TokenType_eof = 0,

    TokenType_real,
    TokenType_str,
    TokenType_bool,

    TokenType_ident,

    TokenType_open_parentheses,
    TokenType_close_parentheses,

    TokenType_plus,
    TokenType_eq,


    TokenType_err, //must be last!
} TokenType;

typedef struct Token_t Token_t;

Token_t* Token_create(TokenType type, const char* ident, size_t col, size_t row);
void     Token_destroy(Token_t* tok);
void     Token_dump(Token_t* tok);

typedef struct Lexer_t Lexer_t;

Lexer_t* Lexer_create(const char* src);
void     Lexer_destroy(Lexer_t* lex);
Token_t* Lexer_nextToken(Lexer_t* lex);
size_t   Lexer_lexAll(Lexer_t* lex);

#endif