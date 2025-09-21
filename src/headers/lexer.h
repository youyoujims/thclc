#ifndef LEXER_HEADER
#define LEXER_HEADER
#include <stdio.h>
#include <stdbool.h>
#include "token.h"

typedef struct LEXER_STRUCT{
    char* source;
    char* file_path;
    int cur, col, row;

}lexer;

lexer* lexer_init(char* file_path, char* source);
void lexer_free(lexer* lexer);

char lexer_chop_char(lexer* lexer, char* source, int pos);
bool is_empty(lexer* lexer, char* source, int pos);
void lexer_trim_left(lexer* lexer, char* source, int pos);

token* lexer_next_token(lexer* lexer);
token* lexer_next_token_(lexer* lexer, char* source, int pos);
token* lexer_next_token_source(lexer* lexer, char* source, int pos);

#endif
