
#include "../include/lexer.h"

static const char* TokenType_to_cstr[TokenType_err + 1] = {
    [TokenType_eof] =  "eof",
    [TokenType_real] =  "real",
    [TokenType_str] =  "str",
    [TokenType_bool] =  "bool",
    [TokenType_ident] =  "ident",
    [TokenType_open_parentheses] =  "open_per",
    [TokenType_close_parentheses] = "close_per", 
    [TokenType_plus] =  "plus",
    [TokenType_eq] =  "eq",
    [TokenType_err] =  "err",
};

struct Token_t {
    TokenType type;
    const char* ident;
    size_t col;
    size_t row;
};

Token_t* Token_create(TokenType type, const char* ident, size_t col, size_t row){
    Token_t* res = (Token_t*)malloc(sizeof(*res));
    if(!res) run_time_error("no system memory");
    res->type = type;
    res->ident = ident;
    res->col = col;
    res->row = row;
    return res;
}

void     Token_destroy(Token_t* tok){
    free((char*)tok->ident);
    free((Token_t*)tok);
}

struct Lexer_t {
    const char* src;
    const char* start;
    const char* end;
    size_t col;
    size_t row;
};

Lexer_t* Lexer_create(const char* src){
    Lexer_t* res = (Lexer_t*)malloc(sizeof(*res));
    if(!res) run_time_error("no system memory");
    res->src = src;
    res->start = src;
    res->end = src;
    res->col = 1;
    res->row = 1;
    return res;
}

void   Lexer_destroy(Lexer_t* lex){
    free(lex);
}

static void Lexer_trim(Lexer_t* lex) {
    while (*lex->end != '\0' && isspace((unsigned char)*lex->end)) {
        if (*lex->end == '\n') {
            lex->row++;
            lex->col = 1;
        } else {
            lex->col++;
        }
        lex->end++;
    }
    lex->start = lex->end;
}

static void Lexer_lexComment(Lexer_t* lex){
    while(*lex->end != '\0' && *lex->end != '\n'){
        lex->end++;
        lex->col++;
    }
    if (*lex->end == '\n') {
        lex->end++;
        lex->row++;
        lex->col = 1;
    }
    lex->start = lex->end;
}

static Token_t* Lexer_lexReal(Lexer_t* lex){
    char* end;
    (void)strtod(lex->end, &end);
    if(end == lex->end)
        run_time_error("failed to parse real at row %zu col %zu", lex->row, lex->col);

    // create string for token text
    size_t len = end - lex->end;
    char* text = malloc(len + 1);
    if(!text) run_time_error("no system memory");
    strncpy(text, lex->end, len);
    text[len] = '\0';

    // update lexer position
    lex->end = end;
    lex->start = end;
    lex->col += len;

    return Token_create(TokenType_real, text, lex->col - len, lex->row);
}

static Token_t* Lexer_lexIdent(Lexer_t* lex) {
    const char* start = lex->end;
    while (isalnum(*lex->end) || *lex->end == '_') lex->end++;
    size_t len = lex->end - start;

    char* text = malloc(len + 1);
    if(!text) run_time_error("no system memory");
    strncpy(text, start, len);
    text[len] = '\0';

    lex->start = lex->end;
    lex->col += len;

    if(strcmp(text, "True") == 0 || strcmp(text, "False") == 0) {
        return Token_create(TokenType_bool, text, lex->col - len, lex->row);
    }

    return Token_create(TokenType_ident, text, lex->col - len, lex->row);
}

static Token_t* Lexer_lexStr(Lexer_t* lex) {
    if(*lex->end != '"') run_time_error("expected '\"' at row %zu col %zu", lex->row, lex->col);
    lex->end++; // skip opening quote
    const char* start = lex->end;

    while(*lex->end != '\0' && *lex->end != '"') {
        lex->end++;
        lex->col++;
    }
    if(*lex->end != '"') run_time_error("unterminated string at row %zu col %zu", lex->row, lex->col);

    size_t len = lex->end - start;
    char* text = malloc(len + 1);
    if(!text) run_time_error("no system memory");
    strncpy(text, start, len);
    text[len] = '\0';

    lex->end++; // skip closing quote
    lex->start = lex->end;
    lex->col++;

    return Token_create(TokenType_str, text, lex->col - len - 1, lex->row);
}

static Token_t* Lexer_lexSymble(Lexer_t* lex) {
    char c = *lex->end;

    // allocate string for ident
    char* ident = malloc(2);
    if(!ident) run_time_error("no system memory");
    ident[0] = c;
    ident[1] = '\0';

    lex->end++;
    lex->col++;

    switch(c) {
        case '+': return Token_create(TokenType_plus, ident, lex->col - 1, lex->row);
        case '=': return Token_create(TokenType_eq, ident, lex->col - 1, lex->row);
        case '(': return Token_create(TokenType_open_parentheses, ident, lex->col - 1, lex->row);
        case ')': return Token_create(TokenType_close_parentheses, ident, lex->col - 1, lex->row);
        default:  return Token_create(TokenType_err, ident, lex->col - 1, lex->row);
    }
}

Token_t* Lexer_nextToken(Lexer_t* lex){
    if(!lex) run_time_error("null lexer in: `%s`", __func__);

    Lexer_trim(lex);
    while (*lex->end == '#') {
        Lexer_lexComment(lex);
        Lexer_trim(lex);
    }

    char c = *lex->end;
    
    if(c == '\0'){
        return Token_create(TokenType_eof, NULL, lex->col, lex->row);
    }
    
    else if(isdigit(c)){
        return Lexer_lexReal(lex);
    }

    else if(isalpha(c) || c == '_'){
        return Lexer_lexIdent(lex);  // or Bool!
    }

    else if(c == '"'){
        return Lexer_lexStr(lex);
    }

    else{
        return Lexer_lexSymble(lex);  // or error!
    }

}

size_t Lexer_lexAll(Lexer_t* lex) {
    size_t count = 0;
    Token_t t = {0};
    Token_t* tok = &t;
    tok->type = TokenType_err;
    while(tok->type != TokenType_eof){
        tok = Lexer_nextToken(lex);
        printf("Token: type: `%s` text='%s' row=%zu col=%zu\n", 
            TokenType_to_cstr[tok->type], 
            tok->ident, 
            tok->row, 
            tok->col);
        count++;
    }
    return count;
}
