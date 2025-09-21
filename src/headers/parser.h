#ifndef PARSER_HEADER
#define PARSER_HEADER
#include "generator.h"
#include "ast.h"
#include "lexer.h"
#include "vars_list.h"
#include "token_list.h"
#include "function.h"
#include "statement.h"

/*
typedef struct FUNCTION_STRUCT {
    char* name;
    enum{
        FUNCTION_INT,
        FUNCTION_VOID,
        FUNCTION_STRING,
        FUNCTION_CHAR
    }type;
    }function;*/


void parser_free_ast_node(ASTNode* node);

function* parser_function_init(char* name, int type);
statement* parser_statement_init(function* function, char* name, char* args, int type, int token_type, ASTNode* expr);
int parser_parse(generator* generator, lexer* lexer, vars_list* vars);
function* parser_parse_function(generator* generator, lexer* lexer, vars_list* vars);
void parser_parse_statement(lexer* lexer, generator* generator, function* function, char* block, int block_pos, vars_list* vars, int type, char* name, char** section);
void parser_free_function(function* function);
void parser_free_statement(statement* statement);

int parser_expect_token(lexer* lexer, token* cur, int type, char* str);
void parser_parse_block(lexer* lexer, generator* generator, function* function, vars_list* vars);

void clean_exit(lexer* lexer, generator* generator, function* function, vars_list* vars, char* name);

#endif
