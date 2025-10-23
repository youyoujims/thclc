#include "headers/generator.h"
#include "headers/vars_list.h"
#include "headers/token.h"
#include "headers/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

generator* generator_init(){
    generator* generator = malloc(sizeof(*generator));
    generator->output = calloc(1, sizeof(char));
    generator->output[0] = '\0';

    generator->rsp = list_init();

    generator->stack = calloc(1, sizeof(char));
    generator->stack[0] = '\0';
    generator->id = 0;
    
    generator->section_data = calloc(1, sizeof(char));
    generator->section_data[0] = '\0';
    generator_append(&generator->section_data, ".data\n");
    
    generator->section_rodata = calloc(1, sizeof(char));
    generator->section_rodata[0] = '\0';
    //generator_append(&generator->section_rodata, ".section .rodata\n");


    generator->jmps = calloc(1, sizeof(char));
    generator->jmps[0] = '\0';
    generator->jmps_len = 0;

    generator_append(&generator->output, ".intel_syntax noprefix\n");
    generator_append(&generator->output, ".global _start\n");

    generator_append(&generator->output, ".text\n");
    generator_append(&generator->output, "_start:\n");
    return generator;
}

void generator_free(generator* generator){
    list_free(generator->rsp);
    free(generator->output);
    free(generator->jmps);
    free(generator->section_data);
    free(generator->section_rodata);
    free(generator->stack);
    free(generator);

}
void generator_append(char** destination, char* str){
    size_t current_len = *destination ? strlen(*destination) : 0;
    size_t new_size = (current_len + strlen(str))+1;

    char* output = realloc(*destination, new_size);
    if(!output){
        exit(1);
    }
    *destination = output;
    strcat(*destination + current_len, str);
}

int get_address(generator* generator, int rsp_address){
    return ((generator->rsp->len-1) - (rsp_address));
}

void generator_parse_statement_return(generator* generator, statement* statement, vars_list* vars, char** section){
    char* output = ""; 
    if(statement->token_type == TOKEN_INT_LIT) {
        //generator_append(section, "    mov rax, 60\n");
        generator_parse_node(generator, statement->expr, vars);

        generator_append(section, "    mov rax, 60\n");
        generator_append(section, "    pop rdi\n"); 

        generator_append(section, "    syscall\n");
        generator_append(section, "\n");

    }else if(statement->token_type == TOKEN_IDENTIFIER){
        generator_append(section, "    mov rax, 60\n");

        char* rsp_str = list_get(generator->rsp, statement->args);
        int rsp_address = atoi(rsp_str);

        generator_parse_node(generator, statement->expr, vars);


        output = (get_address(generator, rsp_address) >= 1)
            ? generator_ree("    push QWORD ptr[rsp+%d]\n", (get_address(generator, rsp_address) * 8))
            : generator_ree("    push QWORD ptr[rsp]\n");

        generator_append(section, output);
        generator_append(section, "    pop rdi\n");
        generator_append(section, "    syscall\n");
        generator_append(section, "\n");
        free(rsp_str);
        free(output);
    }
}

void generator_parse_statement_print(generator* generator, statement* statement, vars_list* vars, char** section){
    char* output = "";
    if(statement->token_type == TOKEN_STRING_LIT){
        //id++ and add to the stack
        generator_append(section, "    mov rax, 1\n");
        generator_append(section, "    mov rdi, 1\n");
        output = generator_ree("    lea rsi, [id_%d]\n", generator->id);
        generator_append(section, output);
        free(output);

        output = generator_ree("    mov rdx, %zu\n", strlen(statement->args));
        generator_append(section, output);
        generator_append(section, "    syscall\n\n");
        free(output);

        output = generator_ree("id_%d:\n    .asciz \"%s\"\n", generator->id, statement->args);
        generator_append(&generator->stack, output);
        free(output);
        generator->id++;
    }else if(statement->token_type == TOKEN_IDENTIFIER){
        generator_append(section, "    mov rax, 1\n");
        generator_append(section, "    mov rdi, 1\n");
        output = generator_ree("    lea rsi, [rip+%s]\n", statement->args);
        generator_append(section, output);
        free(output);

        output = generator_ree("    mov edx, DWORD PTR [rip + __%s_len]\n", statement->args);
        generator_append(section, output);
        generator_append(section, "    syscall\n\n");
        free(output);
    }
}

void generator_parse_statement_init(generator* generator, statement* statement, vars_list* vars, char** section){
    char* output = "";
    //verify the type of the variable
    if(list_type_equals_to(vars, statement->name, "string") == 1){
        output = generator_ree("    %s: .space %d\n    __%s_len: .long 0\n", statement->name, strlen(statement->args),
                statement->name);
        generator_append(&generator->section_data, output);
        free(output);

        output  = generator_ree("    lea rdi, [rip+%s]\n", statement->name);
        generator_append(section, output);
        free(output);

        int i = 0;
        int contains_schar = 1;
        while(i < strlen(statement->args)){

            if(statement->args[i] == '\\'){
                output = generator_ree("    mov byte ptr [rdi+%d], '\\%c'\n", i, statement->args[i+1]);
                i++;
                contains_schar = 0;
            }else{
                output = generator_ree("    mov byte ptr [rdi+%d], '%c'\n", i, statement->args[i]);
                contains_schar = 1;
            }
            generator_append(section, output);
            free(output);
            i++;
        }        
        output = generator_ree("    mov byte ptr [rdi+%d], 0\n", (contains_schar == 0) ? i-1 : i);
        generator_append(section, output);
        free(output);

        output = generator_ree("    mov DWORD PTR[rip + __%s_len], %d\n", statement->name, strlen(statement->args));
        generator_append(section, output);

        free(output);
        generator_append(section, "\n");
    }else if(list_type_equals_to(vars, statement->name, "int") == 1){
        if(statement->has_expr != true){
            return;
        }
        output = generator_ree("%s %d", statement->name, generator->rsp->len);
        list_append(generator->rsp, output);
        free(output);
        generator_parse_node(generator, statement->expr, vars);

    }

}

void generator_parse_node(generator* generator, ASTNode* node, vars_list* vars){
    if(!node){
        return;
    }
    char* output = NULL;
    if(node->type == NODE_LITERAL){
        output = generator_ree("    mov rax, %d\n", node->value);
        generator_append(&generator->output, output); 
        free(output);
        
        generator_append(&generator->output, "    push rax\n");
        generator_append(&generator->output, "\n");
    }else if(node->type == NODE_BINARY_OP){
        generator_parse_node(generator, node->left, vars);
        generator_parse_node(generator, node->right, vars);

        generator_append(&generator->output, "    pop rbx\n"); 
        generator_append(&generator->output, "    pop rax\n"); 

        if(node->op_type == TOKEN_PLUS){
            generator_append(&generator->output, "    add rax, rbx\n"); 
        }if(node->op_type == TOKEN_MINUS){
            generator_append(&generator->output, "    sub rax, rbx\n"); 
        }

        generator_append(&generator->output, "    push rax\n");
        generator->rsp->len++; //                                           < ----- 

        generator_append(&generator->output, "\n");
    }else if(node->type == NODE_VARIABLE){
        char* rsp_str = list_get(generator->rsp, node->name);
        int rsp_address  = atoi(rsp_str);

        output = (get_address(generator, rsp_address) >= 1) ? generator_ree("    mov rax, qword ptr[rsp+%d]\n", (get_address(generator, rsp_address) * 8))
            : generator_ree("    mov rax, qword ptr[rsp]\n");
        generator_append(&generator->output, output); 
        free(output);
        
        generator_append(&generator->output, "    push rax\n");
        generator_append(&generator->output, "\n");
    }


}

void generator_parse_statement_init_const(generator* generator, statement* statement, vars_list* vars){
    char* output = "";
    output = generator_ree("    %s: .ascii \"%s\"\n    __%s_len: .long .-%s\n", statement->name, statement->args,
            statement->name, statement->name);
    generator_append(&generator->section_rodata, output);
    free(output);
    generator_append(&generator->output, "\n");
}

void generator_parse_statement_reassignement(generator* generator, statement* statement, vars_list* vars){
    char* output = "";
    if(list_type_equals_to(vars, statement->name, "string") == 1){
        output = generator_ree("    lea rdi, [rip+%s]\n", statement->name);
        generator_append(&generator->output, output);
        free(output);
        int i = 0;
        int contains_schar = 1;

        while(i < strlen(statement->args)){
            if(statement->args[i] == '\\'){
                output = generator_ree("    mov byte ptr [rdi+%d], '\\%c'\n", i, statement->args[i+1]);
                i++;
                contains_schar = 0;
            }else{
                output = generator_ree("    mov byte ptr [rdi+%d], '%c'\n", i, statement->args[i]);
                contains_schar = 1;
            }
            generator_append(&generator->output, output);
            free(output);
            i++;
        }
        output = generator_ree("    mov byte ptr [rdi+%d], 0\n", (contains_schar == 0) ? i-1 : i);
        generator_append(&generator->output, output);
        free(output);

        output = generator_ree("    mov DWORD PTR[rip + __%s_len], %d\n", statement->name, strlen(statement->args));
        generator_append(&generator->output, output);
        free(output);				
        generator_append(&generator->output, "\n");          

    }else if(list_type_equals_to(vars, statement->name, "int") == 1){
        generator_parse_node(generator, statement->expr, vars);
    }
}

char* get(int id, char* str){
    int i = 0;
    int cur_id = 0;

    while(str[i] && isspace(str[i]))i++;
    while(str[i] && cur_id < id){
        while(str[i] && !isspace(str[i])) i++; //jumping a word
        while(str[i] && isspace(str[i])) i++; // jumping spaces
        cur_id++;
    }
    if(!str[i]) return NULL;

    int start = i;
    while(str[i] && !isspace(str[i]))i++;
    int len = i - start;
    
    char* result = malloc(len + 1);
    strncpy(result, &str[start], len);
    result[len] = '\0';
    return result;
}

void generator_parse_statement_condition(generator* generator, statement* statement, vars_list* vars){
    char* output = NULL;

    output = generator_ree("    mov rax, %d\n", statement->expr->right->value);
    generator_append(&generator->output, output);
    free(output);
    
    char* rsp_str = list_get(generator->rsp, statement->expr->left->name);
    int rsp_address  = atoi(rsp_str);
    output = (get_address(generator, rsp_address) >= 1) ? generator_ree("    cmp qword ptr[rsp+%d], rax\n", (get_address(generator, rsp_address) * 8))
        : generator_ree("    cmp qword ptr[rsp], rax\n");
    generator_append(&generator->output, output);
    free(output);

    if(statement->expr->op_type == TOKEN_BIGGER){
        output = generator_ree("    ja _if_%d_success\n", generator->jmps_len);
        generator_append(&generator->output, output);
        free(output);

        output = generator_ree("_if_%d_success:\n", generator->jmps_len);
        generator_append(&generator->jmps, output);
        free(output);
        generator->jmps_len++;

    }else if(statement->expr->op_type == TOKEN_LOWER){

        output = generator_ree("    jl _if_%d_success\n", generator->jmps_len);
        generator_append(&generator->output, output);
        free(output);
        
        output = generator_ree("_if_%d_success:\n", generator->jmps_len);
        generator_append(&generator->jmps, output);
        free(output);
        generator->jmps_len++;

    }else if(statement->expr->op_type == TOKEN_IS_EQUAL_TO){

        output = generator_ree("    je _if_%d_success\n", generator->jmps_len);
        generator_append(&generator->output, output);
        free(output);
        
        output = generator_ree("_if_%d_success:\n", generator->jmps_len);
        generator_append(&generator->jmps, output);
        free(output);
        generator->jmps_len++;

    }else if(statement->expr->op_type == TOKEN_NOT_EQUAL){

        output = generator_ree("    jne _if_%d_success\n", generator->jmps_len);
        generator_append(&generator->output, output);
        free(output);
        
        output = generator_ree("_if_%d_success:\n", generator->jmps_len);
        generator_append(&generator->jmps, output);
        free(output);
        generator->jmps_len++;

    }

    free(rsp_str);
    generator_append(&generator->output, "\n"); 
}

void generator_statement_to_asm(generator* generator, statement* statement, vars_list* vars, char** section){
    switch(statement->type){
        case STATEMENT_RETURN:
            generator_parse_statement_return(generator, statement, vars, section);
            break;
        case STATEMENT_PRINT:
            generator_parse_statement_print(generator, statement, vars, section);
            break;
        case STATEMENT_INITILISATION:
            generator_parse_statement_init(generator, statement, vars, section);
            break;
        case STATEMENT_INITILISATION_CONST:
            generator_parse_statement_init_const(generator, statement, vars);
            break;
        case STATEMENT_REASSIGNEMENT:
            generator_parse_statement_reassignement(generator, statement, vars);
            break;
        case STATEMENT_CONDITION:
            generator_parse_statement_condition(generator, statement, vars);
            break;
        default: break;
    }    
}

char* generator_ree(const char* format, ...){
    va_list args;
    va_start(args, format);

    int size = vsnprintf(NULL, 0, format, args);
    va_end(args);

    if(size < 0) return NULL;

    char* buffer = malloc(size + 1);
    if(!buffer) return NULL;
    va_start(args, format);
    vsnprintf(buffer, size + 1, format, args);
    va_end(args);

    return buffer;
}

void generator_generate_asm(generator* generator){
    /*generator_append(&generator->output, "    mov rax, 60\n");
    generator_append(&generator->output, "    mov rdi, 0\n");
    generator_append(&generator->output, "    syscall\n");
    printf("%s\n", generator->section_data);
    printf("%s\n", generator->section_rodata);
    printf("%s", generator->output);
    printf("%s", generator->jmps);
    printf("%s\n", generator->stack);
    */
    
    FILE *file;
    file = fopen("assem.s", "w+");
    fprintf(file, generator->section_data);
	fprintf(file, generator->section_rodata);
    fprintf(file, generator->output);
    fprintf(file, generator->jmps);
    fprintf(file, generator->stack);
    fclose(file);
}
