#include "headers/token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token* token_init(char* value, int type){
    token* token = malloc(sizeof(*token));
    token->type = type;
    token->value = calloc(strlen(value)+1, sizeof(char));
    strcpy(token->value, value);

    //printf("[ALLOC] token value debug: %s (%p)\n", token->value, token->value);
    return token;
}

char* token_type_to_string(token* token){
    switch(token->type){
        case TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
        case TOKEN_INT_LIT: return "TOKEN_INT_LIT";
        case TOKEN_RETURN: return "TOKEN_RETURN";
        case TOKEN_INT: return "TOKEN_INT";
		case TOKEN_INT64: return "TOKEN_INT64";
        case TOKEN_FUNCTION: return "TOKEN_FUNCTION";
        case TOKEN_OPAREN: return "TOKEN_OPAREN";
        case TOKEN_CPAREN: return "TOKEN_CPAREN";
        case TOKEN_OCURLY: return "TOKEN_OCURLY";
        case TOKEN_CCURLY: return "TOKEN_CCURLY";
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_VOID: return "TOKEN_VOID";
        case TOKEN_EOF: return "TOKEN_END_OF_FILE";
        case TOKEN_STRING_LIT: return "TOKEN_STRING_LIT";
        case TOKEN_PRINT: return "TOKEN_PRINT";
        case TOKEN_EQUAL: return "TOKEN_EQUAL";
        case TOKEN_PLUS: return "TOKEN_PLUS";
        case TOKEN_MINUS: return "TOKEN_MINUS";
        case TOKEN_STRING: return "TOKEN_STRING";
        case TOKEN_CONST: return "TOKEN_CONST";
        case TOKEN_UNKNOWN: return "TOKEN_UNKNOWN";
        case TOKEN_IF: return "TOKEN_IF";
        case TOKEN_MAINF: return "TOKEN_MAINF";
        case TOKEN_BIGGER: return "TOKEN_BIGGER";
        case TOKEN_LOWER: return "TOKEN_LOWER";
        case TOKEN_NOT_EQUAL: return "TOKEN_NOT_EQUAL";
        case TOKEN_IS_EQUAL_TO: return "TOKEN_IS_EQUAL_TO";
    }
    return "";
} 

void token_free(token* token){
    if(token != NULL){ 
        if (token->value != NULL) {
            free(token->value);
            token->value = NULL;
        }
        free(token);
    }
}
