#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/lexer.h"
#include <stdbool.h>
#include <ctype.h>

lexer* lexer_init(char* file_path, char* source){
    lexer* lexer = malloc(sizeof(*lexer));
    lexer->cur = 0;
    lexer->row = 1;
    lexer->col = 1;
    size_t size = strlen(source) + 1;
    lexer->source = malloc(size * sizeof(char));
    strcpy(lexer->source, source);

    lexer->file_path = malloc((strlen(file_path)+1) * sizeof(char));
    strcpy(lexer->file_path, file_path);
    return lexer;
}

bool is_empty(lexer* lexer, char* source, int pos){
    return !((lexer->cur - pos) < strlen(source));
}

char lexer_chop_char(lexer* lexer, char* source, int pos){
    if((lexer->cur - pos) < strlen(source)){
        char current = source[lexer->cur - pos]; 
        lexer->cur++;
        lexer->col++;
        if(current == '\n'){
            lexer->col = 1;
            lexer->row++;
        }
        return current;
    }
    return '\0';
}

void lexer_trim_left(lexer* lexer, char* source, int pos){
    while(!is_empty(lexer, source, pos) && isspace(source[lexer->cur - pos])){
        lexer_chop_char(lexer, source, pos);
    }
}

token* lexer_next_token_source(lexer* lexer, char* source, int pos){
    return lexer_next_token_(lexer, source, pos);
}

token* lexer_next_token(lexer* lexer){
    return lexer_next_token_(lexer, lexer->source, 0);
}

token* lexer_next_token_(lexer* lexer, char* source, int pos){
    if(source == NULL){
        printf("The source is null\n");
        return token_init("NULL", TOKEN_UNKNOWN);
    }
    lexer_trim_left(lexer, source, pos);

    if(is_empty(lexer, source, pos)) {
        return NULL;
    }
    
    if(isalpha(source[lexer->cur - pos])){
        //printf("%c %c\n", source[lexer->cur - pos], source[lexer->cur]);
        size_t buff_len = 0;
        size_t capacity = 2;
        char* buff = calloc(capacity, sizeof(char));
        while(!is_empty(lexer, source, pos) && isalnum(source[lexer->cur - pos])){ //while it's not and its either a number or a letter
            if(buff_len + 2 > capacity){
                capacity *= 2;
                char* new_buff = realloc(buff, capacity);
                if(new_buff == NULL){
                    free(buff);
                    printf("issue at alphabets buff\n");
                    return NULL;
                }
                buff = new_buff;
            }
            buff[buff_len++] = lexer_chop_char(lexer, source, pos);
            buff[buff_len] = '\0';
        }
        token* token;
        if(strcmp(buff, "function") == 0 || strcmp(buff, "func") == 0){
            token = token_init(buff, TOKEN_FUNCTION);
        }else if(strcmp(buff, "int") == 0){
            token = token_init(buff, TOKEN_INT);
        }else if(strcmp(buff, "int64") == 0){
            token = token_init(buff, TOKEN_INT64);
        }else if(strcmp(buff, "void") == 0){
            token = token_init(buff, TOKEN_VOID);
        }else if(strcmp(buff, "return") == 0){
            token = token_init(buff, TOKEN_RETURN);
        }else if(strcmp(buff, "print") == 0){
            token = token_init(buff, TOKEN_PRINT);
        }else if(strcmp(buff, "string") == 0){
            token = token_init(buff, TOKEN_STRING);
        }else if (strcmp(buff, "const") == 0){
            token = token_init(buff, TOKEN_CONST);

        }else if (strcmp(buff, "if") == 0){
            token = token_init(buff, TOKEN_IF);
        }else if (strcmp(buff, "mainf") == 0){
            token = token_init(buff, TOKEN_MAINF);
        }else{
            token = token_init(buff, TOKEN_IDENTIFIER);
        }
        free(buff);
        return token; //tokeniz'd
    }
    if(isdigit(source[lexer->cur - pos])){
        size_t buff_len = 0;
        size_t capacity = 2;
        char* buff = calloc(capacity, sizeof(char));
        while(!is_empty(lexer, source, pos) && isdigit(source[lexer->cur - pos])){
            if(buff_len + 2 > capacity){
                capacity *= 2;
                char* new_buff = realloc(buff, capacity);
                if(new_buff == NULL){
                    free(buff);
                    printf("issue in the digit buff\n");
                    return NULL;
                }
                buff = new_buff;
            }
            buff[buff_len++] = lexer_chop_char(lexer, source, pos);
            buff[buff_len] = '\0';
        }

        token* token = token_init(buff, TOKEN_INT_LIT);
        free(buff);
        return token;
    }

    if(source[lexer->cur - pos] == '\"'){
        lexer_chop_char(lexer, source, pos);
        size_t buff_len = 0;
        size_t capacity = 2;
        char* buff = calloc(capacity, sizeof(char));

        while(!is_empty(lexer, source, pos) && source[lexer->cur - pos] != '\"'){
            if(buff_len + 2 > capacity){
                capacity *= 2;
                char* new_buff = realloc(buff, capacity);
                if(new_buff == NULL){
                    free(buff);
                    printf("issue in the quote buff\n");
                    return NULL;
                }
                buff = new_buff;
            }
            buff[buff_len++] = lexer_chop_char(lexer, source, pos);
            buff[buff_len] = '\0';
        }
        if(!is_empty(lexer, source, pos) && source[lexer->cur - pos] == '\"'){
            lexer_chop_char(lexer, source, pos);
        }
        token* token = token_init(buff, TOKEN_STRING_LIT);
        free(buff);
        return token;
    }
    if(source[lexer->cur - pos] == '/' && source[lexer->cur - pos + 1] == '/'){
        lexer->cur+=2;
        while(source[lexer->cur - pos] != '\n' && source[lexer->cur - pos] != '\0'){
            lexer->cur++;
        }

        if(source[lexer->cur - pos] == '\n'){
            lexer->cur++;
            lexer->row++;
            lexer->col++;
        }

        return lexer_next_token(lexer);
    }

    if(source[lexer->cur - pos] == '!' && source[lexer->cur - pos + 1] == '='){
        lexer->cur+=2;
        lexer->col+=2;
        return token_init("!=", TOKEN_NOT_EQUAL);
    }

    if(source[lexer->cur - pos] == '=' && source[lexer->cur - pos + 1] == '='){
        lexer->cur+=2;
        lexer->col+=2;
        return token_init("==", TOKEN_IS_EQUAL_TO);
    }
    
    switch(source[lexer->cur - pos]){
        case '(':
            lexer_chop_char(lexer, source, pos);
            return token_init("(", TOKEN_OPAREN);
        case ')':
            lexer_chop_char(lexer, source, pos);
            return token_init(")", TOKEN_CPAREN);
        case '{':
            lexer_chop_char(lexer, source, pos);
            return token_init("{", TOKEN_OCURLY);
        case '}':
            lexer_chop_char(lexer, source, pos);
            return token_init("}", TOKEN_CCURLY);
        case ';':
            lexer_chop_char(lexer, source, pos);
            return token_init(";", TOKEN_SEMICOLON);
        case '+':
            lexer_chop_char(lexer, source, pos);
            return token_init("+", TOKEN_PLUS);
        case '=':
            lexer_chop_char(lexer, source, pos);
            return token_init("=", TOKEN_EQUAL);
        case '-':
            lexer_chop_char(lexer, source, pos);
            return token_init("-", TOKEN_MINUS);
        case '>':
            lexer_chop_char(lexer, source, pos);
            return token_init(">", TOKEN_BIGGER);
        case '<':
            lexer_chop_char(lexer, source, pos);
            return token_init("<", TOKEN_LOWER);
        case '\0':
            lexer_chop_char(lexer, source, pos);
            return token_init("END OF FILE", TOKEN_EOF);
        default:
            lexer_chop_char(lexer, source, pos);
            return token_init("UNKNOWN", TOKEN_UNKNOWN);
    }

}

void lexer_free(lexer* lexer){
     free(lexer->source);
     free(lexer->file_path);
     free(lexer);
} 
