#include "headers/parser.h"
#include "headers/generator.h"
#include "headers/vars_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

function* parser_function_init(char* name, int type){
    function* function = malloc(sizeof(*function));
    function->name = malloc((strlen(name)+1) * sizeof(char));
    strcpy(function->name, name);
    function->type = type;
    return function;
}

statement* parser_statement_init(function* function, char* name, char* args, int type, int token_type, ASTNode* expr){
    statement* statement = malloc(sizeof(struct STATEMENT_STRUCT));
    statement->function = function;

    statement->name = calloc(strlen(name)+1, sizeof(char));
    strcpy(statement->name, name);
    statement->args = calloc(strlen(args)+1, sizeof(char));
    strcpy(statement->args, args);

    statement->type = type;
    statement->token_type = token_type;

    statement->expr = NULL;
    statement->has_expr = false;

    if(expr != NULL) {
        statement->expr = expr;
        statement->has_expr = true;
    }
    return statement;
}

void parser_free_ast_node(ASTNode* node){
    if(node == NULL){
        return;
    }
    if (node->type == NODE_BINARY_OP) {
        if(node->left) parser_free_ast_node(node->left);
        if(node->right) parser_free_ast_node(node->right);
        if(node->parent) parser_free_ast_node(node->parent);
    } else if (node->type == NODE_LITERAL) {
        
    } else if (node->type == NODE_VARIABLE) {
        free(node->name); 
    }
    
    free(node);
}

void parser_free_function(function* function){
    if(function->name) free(function->name);
    if(function->block) free(function->block); 
    free(function);
}

void parser_free_statement(statement* statement){
    if(statement != NULL){
        if(statement->args != NULL){
            free(statement->args);
        }
        
        if(statement->name != NULL){
            free(statement->name);
        }

        free(statement);
    }

    
}

void clean_exit(lexer* lexer, generator* generator, function* function, vars_list* vars, char* name){
    if(lexer) lexer_free(lexer);
    if(generator) generator_free(generator);
    if(function) parser_free_function(function);
    if(vars) list_free(vars);
    if(name) free(name);
    exit(1);
}

int parser_expect_token(lexer* lexer, token* cur, int type, char* name){
    if(cur->type != type){
        printf(name, cur->value, lexer->file_path, lexer->row, lexer->col);
        token_free(cur);
        clean_exit(lexer, NULL, NULL, NULL, name);
        return 1;
    }else{
        return 0;
    }
}

ASTNode* parser_parse_expression(lexer* lexer, function* function, generator* generator, char* block, int block_pos){
    token* cur = lexer_next_token_source(lexer, block, block_pos);

    if(!cur || (cur->type != TOKEN_INT_LIT && cur->type != TOKEN_IDENTIFIER)){
        parser_expect_token(lexer, cur, TOKEN_INT_LIT, "Expected an integer instead of `%s` %s:%d:%d\n");
        return NULL;
    }
    ASTNode* left = NULL; 
    if(cur->type == TOKEN_INT_LIT){
        left = create_literal_node(atoi(cur->value));
    }else if(cur->type == TOKEN_IDENTIFIER){
        left = create_variable_node(cur->value);
    }
    token_free(cur);

    while(1){
        cur = lexer_next_token_source(lexer, block, block_pos);
        if(!cur) break;

        if(cur->type != TOKEN_PLUS && cur->type != TOKEN_MINUS){
            token_free(cur);
            break;
        }

        int op = cur->type;
        token_free(cur);

        cur = lexer_next_token_source(lexer, block, block_pos);
        if(!cur || (cur->type != TOKEN_INT_LIT && cur->type != TOKEN_IDENTIFIER)){
            parser_expect_token(lexer, cur, TOKEN_INT_LIT, "Expected an integer or an identifier instead of `%s` %s:%d:%d\n");
            return NULL;
        }


        ASTNode* right = NULL; 
        if(cur->type == TOKEN_INT_LIT){
            right = create_literal_node(atoi(cur->value));
        }else if(cur->type == TOKEN_IDENTIFIER){
            right = create_variable_node(cur->value);
        }

        token_free(cur);

        ASTNode* parent = create_binary_op_node(left, right, op);

        left = parent;
    }

    
    return left;
}

void parser_parse_if_statement(generator* generator, lexer* lexer, function* function, vars_list* vars, token* cur){
    switch(cur->type) {
        case TOKEN_RETURN: parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_RETURN, cur->value, &generator->jmps); 
                           break;
        case TOKEN_PRINT: parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_PRINT, cur->value, &generator->jmps); 
                          break;
        case TOKEN_INT:
                          parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_INITILISATION, cur->value, &generator->jmps);
                          break;
        case TOKEN_INT64:
                          parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_INITILISATION, cur->value, &generator->jmps);
                          break;
        case TOKEN_STRING:
                          parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_INITILISATION, cur->value, &generator->jmps);
                          break;
        case TOKEN_CONST:
                          parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_INITILISATION_CONST, cur->value, &generator->jmps);
                          break;
        case TOKEN_IDENTIFIER:
                          parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_REASSIGNEMENT, cur->value, &generator->jmps);
                          break;
        case TOKEN_IF:
                          parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_CONDITION, cur->value, &generator->jmps);
                          break;
        case TOKEN_CCURLY:
                          break;                                                                                                    
                          //case TOKEN_UNKNOWN:  break;
        default: break;
    }

}

void parser_parse_statement(lexer* lexer, generator* generator, function* function, char* block, int block_pos, vars_list* vars, int type, char* name, char** section){
    token* cur = lexer_next_token_source(lexer, block, block_pos);;
    statement* statement = NULL;

    if(strcmp(name, "return") == 0){
        if(cur->type == TOKEN_INT_LIT){
            parser_expect_token(lexer, cur, TOKEN_INT_LIT, "Expected a integer instead of `%s` at %s:%d:%d\n");
            char* value_copy = strdup(cur->value);
            if(value_copy == NULL){
                fprintf(stderr, "Memory error");
                token_free(cur);
                exit(1);
             }
             statement = parser_statement_init(function, name, value_copy, type, TOKEN_INT_LIT, NULL);
             free(value_copy);
             token_free(cur);
        }else if(cur->type == TOKEN_IDENTIFIER){
            parser_expect_token(lexer, cur, TOKEN_IDENTIFIER, "Expected a variable instead of `%s` at %s:%d:%d\n");
            char* value_copy = strdup(cur->value);
            statement = parser_statement_init(function, name, value_copy, type, TOKEN_IDENTIFIER, NULL);
            free(value_copy);
			token_free(cur);
        }

        cur = lexer_next_token_source(lexer, block, block_pos);;
        parser_expect_token(lexer, cur, TOKEN_SEMICOLON, "Expected a `;` instead of `%s` at %s:%d:%d\n");
        token_free(cur);
        
//        statement = parser_statement_init(function, name, value_copy, type, TOKEN_RETURN);
//        free(value_copy);

    }else if(strcmp(name, "print") == 0){

        parser_expect_token(lexer, cur, TOKEN_OPAREN, "Expected a `(` instead of `%s` at %s:%d:%d\n");
        token_free(cur);
        cur = lexer_next_token_source(lexer, block, block_pos);;
        if(cur->type == TOKEN_IDENTIFIER){
            parser_expect_token(lexer, cur, TOKEN_IDENTIFIER, "Expected a identifier instead of `%s` at %s:%d:%d\n");
            statement = parser_statement_init(function, name, cur->value, type, TOKEN_IDENTIFIER, NULL);
            token_free(cur);
        }else if(cur->type == TOKEN_STRING_LIT){
            if(parser_expect_token(lexer, cur, TOKEN_STRING_LIT, "Expected a string instead of `%s` at %s:%d:%d\n") == 0){
                statement = parser_statement_init(function, name, cur->value, type, TOKEN_STRING_LIT, NULL);
                token_free(cur);
            }
        }
        
        cur = lexer_next_token_source(lexer, block, block_pos);;
        parser_expect_token(lexer, cur, TOKEN_CPAREN, "Expected a `)` instead of `%s` at %s:%d:%d\n");
        token_free(cur);

        cur = lexer_next_token_source(lexer, block, block_pos);;
        parser_expect_token(lexer, cur, TOKEN_SEMICOLON, "Expected a `;` instead of `%s` at %s:%d:%d\n");
        token_free(cur);

    }else if(strcmp(name, "int") == 0){
        
        char* variable_name = NULL;
        parser_expect_token(lexer, cur, TOKEN_IDENTIFIER, "Expected an identifier instead of `%s` at %s:%d:%d\n");
        variable_name = calloc((strlen(cur->value)+1), sizeof(char));
        strcpy(variable_name, cur->value);
        token_free(cur);

        cur = lexer_next_token_source(lexer, block, block_pos);
        parser_expect_token(lexer, cur, TOKEN_EQUAL, "Expected a `=` instead of `%s` at %s:%d:%d\n");
        token_free(cur);


        ASTNode* expr = parser_parse_expression(lexer, function, generator, block, block_pos);
        //parser_free_ast_node(expr);
        statement = parser_statement_init(function, variable_name, variable_name, type, TOKEN_INT, expr); 
        //parser_free_ast_node(expr);
        //parser_free_statement(statement);
       
        char* str = "int %s";
        size_t size = snprintf(NULL, 0, str, variable_name) + 1;
        char* stack = calloc(size, sizeof(char));
        sprintf(stack, str, variable_name);
        list_append(vars, stack);

        free(variable_name);
        free(stack);

    }else if(strcmp(name, "string") == 0){

        parser_expect_token(lexer, cur, TOKEN_IDENTIFIER, "Expected an identifier instead of `%s` at %s:%d:%d\n");
        char* variable_name = calloc((strlen(cur->value)+1), sizeof(char));
        strcpy(variable_name, cur->value);
        token_free(cur);

        cur = lexer_next_token_source(lexer, block, block_pos);;
        parser_expect_token(lexer, cur, TOKEN_EQUAL, "Expected a `=` instead of `%s` at %s:%d:%d\n");
        token_free(cur);

        cur = lexer_next_token_source(lexer, block, block_pos);;
        parser_expect_token(lexer, cur, TOKEN_STRING_LIT, "Expected a string instead of `%s` at %s:%d:%d\n");
        statement = parser_statement_init(function, variable_name, cur->value, type, TOKEN_STRING, NULL);
        char* str = "string %s";
        size_t size = snprintf(NULL, 0, str, variable_name) + 1;
        char* stack = calloc(size, sizeof(char));
        sprintf(stack, str, variable_name);
        list_append(vars, stack);
        token_free(cur);

        cur = lexer_next_token_source(lexer, block, block_pos);;
        parser_expect_token(lexer, cur, TOKEN_SEMICOLON, "Expected a `;` instead of `%s` at %s:%d:%d\n");
        token_free(cur);

        free(variable_name);
        free(stack);

    }else if(strcmp(name, "const") == 0){
        
        if(cur->type == TOKEN_STRING){
            parser_expect_token(lexer, cur, TOKEN_STRING, "Expected a string type instead of `%s` at %s:%d:%d\n");
            token_free(cur);
            
            cur = lexer_next_token_source(lexer, block, block_pos);;
            parser_expect_token(lexer, cur, TOKEN_IDENTIFIER, "Expected an identifier instead of `%s` at %s:%d:%d\n");
            char* variable_name = calloc((strlen(cur->value)+1), sizeof(char));
            strcpy(variable_name, cur->value);
            token_free(cur);

            cur = lexer_next_token_source(lexer, block, block_pos);;
            parser_expect_token(lexer, cur, TOKEN_EQUAL, "Expected a `=` instead of `%s` at %s:%d:%d\n");
            token_free(cur);

            cur = lexer_next_token_source(lexer, block, block_pos);;
            parser_expect_token(lexer, cur, TOKEN_STRING_LIT, "Expected a string instead of `%s` at %s:%d:%d\n");
            statement = parser_statement_init(function, variable_name, cur->value, type, TOKEN_STRING, NULL);
            char* str = "const_string %s";
            size_t size = snprintf(NULL, 0, str, variable_name) + 1;
            char* stack = calloc(size, sizeof(char));
            sprintf(stack, str, variable_name);
            list_append(vars, stack);
            token_free(cur);

            cur = lexer_next_token_source(lexer, block, block_pos);;
            parser_expect_token(lexer, cur, TOKEN_SEMICOLON, "Expected a `;` instead of `%s` at %s:%d:%d\n");
            token_free(cur);

            free(variable_name);
            free(stack);
        }
    }else if(strcmp(name, "if") == 0){
        parser_expect_token(lexer, cur, TOKEN_OPAREN, "Expected a `(` instead of `%s` at %s:%d:%d\n");
        token_free(cur);

        cur = lexer_next_token_source(lexer, block, block_pos);;
        parser_expect_token(lexer, cur, TOKEN_IDENTIFIER, "Expected a variable instead of `%s` at %s:%d:%d\n");
        char* variable_name = calloc((strlen(cur->value)+1), sizeof(char));
        strcpy(variable_name, cur->value);
        token_free(cur);
        
        
        cur = lexer_next_token_source(lexer, block, block_pos);;
        int op_type = 0;
        if(cur->type == TOKEN_BIGGER || cur->type == TOKEN_LOWER || 
           cur->type == TOKEN_NOT_EQUAL || cur->type == TOKEN_IS_EQUAL_TO){
            op_type = cur->type;
            token_free(cur);
        }else{
            printf("Expected a `<` or `>` or `!=` or `==` instead of `%s` at %s:%d:%d\n", cur->value, lexer->file_path, lexer->row, lexer->col);
            token_free(cur);
            clean_exit(lexer, generator, function, vars, name);
        }
        
        cur = lexer_next_token_source(lexer, block, block_pos);;
        parser_expect_token(lexer, cur, TOKEN_INT_LIT, "Expected an integer instead of `%s` at %s:%d:%d\n");

        ASTNode* left_node = create_variable_node(variable_name);
        ASTNode* right_node = create_literal_node(atoi(cur->value));
        ASTNode* expr = create_binary_op_node(left_node, right_node, op_type);
            /*
        size_t len = snprintf(NULL, 0, "%s %s %s", variable_name, op_type, cur->value) + 1;
        char* args = calloc(len, sizeof(char));
        snprintf(args, len, "%s %s %s", variable_name, op_type, cur->value);*/

        statement = parser_statement_init(function, name, "bro", type, TOKEN_IF, expr);
        token_free(cur);
        
        cur = lexer_next_token_source(lexer, block, block_pos);;
        parser_expect_token(lexer, cur, TOKEN_CPAREN, "Expected a `)` instead of `%s` at %s:%d:%d\n");
        token_free(cur);

        cur = lexer_next_token_source(lexer, block, block_pos);;
        parser_expect_token(lexer, cur, TOKEN_OCURLY, "Expected a `{` instead of `%s` at %s:%d:%d\n");
        token_free(cur);

        generator_statement_to_asm(generator, statement, vars, section);
        parser_free_statement(statement);   

        statement = NULL;

        //while cur isn't a `}`
        while((cur = lexer_next_token_source(lexer, block, block_pos)) != NULL){
            if(cur->type == TOKEN_CCURLY){
                token_free(cur);
                break;
            }
            parser_parse_if_statement(generator, lexer, function, vars, cur);
            
            token_free(cur);
       }

        free(variable_name);
        //free(args);
    }

    if(type == STATEMENT_REASSIGNEMENT){
        if(list_contains(vars, name) == 0){
            if(list_is_constant(vars, name)){
                printf("The variable `%s` is a constant at %s:%d:%d\n", name, lexer->file_path, lexer->row, lexer->col);
                printf("A constant variable can not be changed\n");
                token_free(cur);
                parser_free_statement(statement);
                clean_exit(lexer, generator, function, vars, name);
            }
            if(list_type_equals_to(vars, name, "string")){
                parser_expect_token(lexer, cur, TOKEN_EQUAL, "Expected a `=` instead of `%s` at %s:%d:%d\n");
                token_free(cur);

                cur = lexer_next_token_source(lexer, block, block_pos);;
                parser_expect_token(lexer, cur, TOKEN_STRING_LIT, "Expected a string type instead of `%s` at %s:%d:%d\n");
                statement = parser_statement_init(function, name, cur->value, type, TOKEN_STRING_LIT, NULL);
                token_free(cur);

                cur = lexer_next_token_source(lexer, block, block_pos);;
                parser_expect_token(lexer, cur, TOKEN_SEMICOLON, "Expected a `;` instead of `%s` at %s:%d:%d\n");
                token_free(cur);
            }else if(list_type_equals_to(vars, name, "int")){

                parser_expect_token(lexer, cur, TOKEN_EQUAL, "Expected a `=` instead of `%s` at %s:%d:%d\n");
                token_free(cur);


                cur = lexer_next_token_source(lexer, block, block_pos);;

                char* left = NULL;
                if(cur->type == TOKEN_INT_LIT){
                    parser_expect_token(lexer, cur, TOKEN_INT_LIT, "Expected a int type instead of `%s` at %s:%d:%d\n");
                    statement = parser_statement_init(function, name, cur->value, type, TOKEN_INT_LIT, NULL);

                    left = strdup(cur->value);
                    if(left == NULL){
                        fprintf(stderr, "memory alloc failed\n");
                        token_free(cur);
                        return;
                    }
                    token_free(cur);
                }else if (cur->type == TOKEN_IDENTIFIER){
                    parser_expect_token(lexer, cur, TOKEN_IDENTIFIER, "Expected an identifier instead of `%s` at %s:%d:%d\n");
                    statement = parser_statement_init(function, name, cur->value, type, TOKEN_IDENTIFIER, NULL); 
                    left = strdup(cur->value);
                    if(left == NULL){
                        fprintf(stderr, "memory alloc failed\n");
                        token_free(cur);
                        return;
                    }

                    token_free(cur);
                }

                cur = lexer_next_token_source(lexer, block, block_pos);;
                if(cur->type == TOKEN_PLUS || cur->type == TOKEN_MINUS){
                    if(left != NULL){
                        ASTNode* expr = parser_parse_expression(lexer, function, generator, block, block_pos);
                        statement = parser_statement_init(function, name, left, type, TOKEN_INT, expr);                       
                        free(left);
                    }else{
                        ASTNode* expr = parser_parse_expression(lexer, function, generator, block, block_pos);
                        statement = parser_statement_init(function, name, left, type, TOKEN_INT, expr);                       
                        free(left);
                    }
                }else{
                    parser_expect_token(lexer, cur, TOKEN_SEMICOLON, "Expected a `;` instead of `%s` at %s:%d:%d\n");
                    token_free(cur);
                    free(left);
                }


            }
        }else{
            printf("Uninitialized variable `%s` %s:%d:%d\n", name, lexer->file_path, lexer->row, lexer->col);
            token_free(cur);
            parser_free_statement(statement);
            clean_exit(lexer, generator, function, vars, name);
        }
    }

    //    if(statement->expr != NULL) printf("%d\n", statement->expr->left->value);

    //make that the generator take the statement and converts it to asm
    if(statement != NULL){
        generator_statement_to_asm(generator, statement, vars, section);
        parser_free_statement(statement);   
    }
}

function* parser_parse_function(generator* generator, lexer* lexer, vars_list* vars){
    token* cur = lexer_next_token(lexer);
    function* function;
    int function_type;
    char* function_name;
    if(cur->type == TOKEN_EOF){
        token_free(cur);
        clean_exit(lexer, generator, NULL, vars, NULL);
    }

    switch(cur->type){
        case TOKEN_VOID: 
            function_type = FUNCTION_VOID;
            break;
        case TOKEN_INT: 
            function_type = FUNCTION_INT;
            break;
        default:
            printf("Expected a type instead of `%s` at %s:%d:%d\n", cur->value, lexer->file_path, lexer->row, lexer->col);
            token_free(cur);
            clean_exit(lexer, generator, NULL, vars, NULL);
            break;
    }
    token_free(cur);

    cur = lexer_next_token(lexer);
    if(cur->type != TOKEN_IDENTIFIER){
        printf("Expected an identifier instead of `%s` at %s:%d:%d\n", cur->value, lexer->file_path, lexer->row, lexer->col);
        token_free(cur);
        clean_exit(lexer, generator, NULL, vars, NULL);
    }
    function_name = malloc((strlen(cur->value)+1) * sizeof(char));
    strcpy(function_name, cur->value);
    token_free(cur);

    function = parser_function_init(function_name, function_type);
    free(function_name);

    //the body remains
    // and the body contains statements (a list)
    cur = lexer_next_token(lexer);
    if(cur->type != TOKEN_OPAREN){
        printf("Expected a `(` instead of `%s` at %s:%d:%d\n", cur->value, lexer->file_path, lexer->row, lexer->col);
        token_free(cur);
        clean_exit(lexer, generator, function, vars, NULL);
    }
    token_free(cur);

    cur = lexer_next_token(lexer);
    if(cur->type != TOKEN_CPAREN){
        printf("Expected a `)` instead of `%s` at %s:%d:%d\n", cur->value, lexer->file_path, lexer->row, lexer->col);
        token_free(cur);
        clean_exit(lexer, generator, function, vars, NULL);
    }
    token_free(cur);
    cur = lexer_next_token(lexer);
    if(cur->type != TOKEN_OCURLY){
        printf("Expected a `{` instead of `%s` at %s:%d:%d\n", cur->value, lexer->file_path, lexer->row, lexer->col);
        token_free(cur);
        clean_exit(lexer, generator, function, vars, NULL);
    }
    token_free(cur);

    function->block = calloc(1, sizeof(char));
    function->block[0] = '\0';
    int len = 0;
    function->block_pos = lexer->cur;
    while((function->block_pos+len < strlen(lexer->source)) && lexer->source[lexer->cur] != EOF){
        if(lexer->source[lexer->cur] == '}'){
           break;
        }
        len++;
        function->block = realloc(function->block, (strlen(function->block)+ len + 1) * sizeof(char));
        char a = lexer->source[function->block_pos + len];
        strcat(function->block, (char[]) {a, '\0'}); //lexer_chop_char(lexer, lexer->source, 0)
    }

    return function;
}

void parser_parse_block(lexer* lexer, generator* generator, function* function, vars_list* vars){
    token* cur;
    while((cur = lexer_next_token_source(lexer, function->block, function->block_pos)) != NULL){
        //printf("%s%s %d:%d\n", cur->value, token_type_to_string(cur), lexer->row, lexer->col);
        if(cur->type == TOKEN_EOF){
            token_free(cur);
            break;
        }

        switch(cur->type) {
            case TOKEN_RETURN: parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_RETURN, cur->value, &generator->output); 
                               token_free(cur);
                               break;
            case TOKEN_PRINT: parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_PRINT, cur->value, &generator->output); 
                              token_free(cur);
                              break;
            case TOKEN_INT:
                              parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_INITILISATION, cur->value, &generator->output);
                              token_free(cur);
                              break;
            case TOKEN_INT64:
                              parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_INITILISATION, cur->value, &generator->output);
                              token_free(cur); 
                              break;
            case TOKEN_STRING:
                              parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_INITILISATION, cur->value, &generator->output);
                              token_free(cur);
                              break;
            case TOKEN_CONST:
                              parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_INITILISATION_CONST, cur->value, &generator->output);
                              token_free(cur);
                              break;
            case TOKEN_IDENTIFIER:
                              parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_REASSIGNEMENT, cur->value, &generator->output);
                              token_free(cur);
                              break;
            case TOKEN_IF:
                              parser_parse_statement(lexer, generator, function, function->block, function->block_pos, vars, STATEMENT_CONDITION, cur->value, &generator->output);
                              token_free(cur);
                              break;
            case TOKEN_CCURLY:
                              token_free(cur);
                              break;                                                                                                    
            //case TOKEN_UNKNOWN: token_free(cur); break;
            default: token_free(cur); break;
        }
    }
    //printf("%d:%d\n", lexer->row, lexer->col);
    parser_free_function(function);
}

int parser_parse(generator* generator, lexer* lexer, vars_list* vars){
    token* cur;
    while((cur = lexer_next_token(lexer)) != NULL){
        if(cur->type == TOKEN_EOF){
            token_free(cur);
            break;
        }

        if(cur->type == TOKEN_FUNCTION) {
            function* function = parser_parse_function(generator, lexer, vars);
            if(strcmp(function->name, "main") != 0){
                printf("Couldn't find the main function\n");
                return 0;
                break;
            }
            parser_parse_block(lexer, generator, function, vars);
        }
        token_free(cur);
    }
    return 1;
}
