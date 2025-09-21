#ifndef GENERATOR_HEADER
#define GENERATOR_HEADER
#include "vars_list.h"
#include "statement.h"
#include "ast.h"
#include "function.h"

//typedef struct ASTNODE ASTNode;

typedef struct GENERATOR_STRUCT{
    char* output;
    char* stack;
    char* section_data;
    char* section_rodata;
    char* jmps;
    int id;
    int jmps_len;
    vars_list* rsp;
}generator;

generator* generator_init();
void generator_statement_to_asm(generator* generator, statement* statement, vars_list* vars, char** section);
char* generator_replace(const char* format, const char* replacement); 
void generator_append(char** destination, char* str);
//void generator_sappend(generator* generator, char* str); //to the stack
//void generator_daappend(generator* generator, char* str); //to the data section 
//void generator_rodaappend(generator* generator, char* str); //to the bss section 
void generator_free(generator* generator);
void generator_parse_node(generator* generator, ASTNode* node, vars_list* vars);
void generator_generate_asm(generator* generator);
void generator_mmap(generator* generator, int size);
char* generator_ree(const char* format, ...);

#endif
