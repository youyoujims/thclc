#ifndef TOKEN_HEADER
#define TOKEN_HEADER

typedef struct TOKEN_STRUCT{
    char* value;
    enum{
        TOKEN_IDENTIFIER,
        TOKEN_INT_LIT,
        TOKEN_RETURN,
        TOKEN_STRING_LIT,
        TOKEN_PRINT,
        TOKEN_INT,
        TOKEN_INT64,
        TOKEN_VOID,
        TOKEN_FUNCTION,
        TOKEN_OPAREN,
        TOKEN_CPAREN,
        TOKEN_OCURLY,
        TOKEN_CCURLY,
        TOKEN_SEMICOLON,
        TOKEN_EQUAL,
        TOKEN_MINUS,
        TOKEN_PLUS,
        TOKEN_EOF,
        TOKEN_IF,
        TOKEN_BIGGER,
        TOKEN_LOWER,
        TOKEN_NOT_EQUAL,
        TOKEN_IS_EQUAL_TO,
        TOKEN_STRING,
        TOKEN_UNKNOWN,
        TOKEN_MAINF,
        TOKEN_CONST
    }type;

}token;

token* token_init(char* value, int type);

char* token_type_to_string(token* token);

void token_free(token* token);

#endif
