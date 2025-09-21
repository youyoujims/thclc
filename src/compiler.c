#include <stdio.h>
#include <stdlib.h>
#include "headers/compiler.h"
#include "headers/lexer.h"
#include "headers/parser.h"
#include "headers/generator.h"
#include "headers/vars_list.h"

char* compiler_read_file(char* file_path){
    FILE* file = fopen(file_path, "r");
    if(file == NULL){
        perror("Couldn't open the file");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    char* buffer = malloc((size+1) * sizeof(char));
    if(buffer == NULL){
        perror("Error malloc");
        fclose(file);
        return NULL;
    }
    fread(buffer, sizeof(char), size, file);
    buffer[size] = '\0';

    fclose(file);
    return buffer;
}

void compiler_compile(char* file_path, char* output_path, bool keep_assembly){
    char* source = compiler_read_file(file_path);
    if(source == NULL){
        exit(1);
    }
    lexer* lexer = lexer_init(file_path, source);
    generator* generator = generator_init();
    vars_list* vars = list_init();

    if(parser_parse(generator, lexer, vars) == 1){
        generator_generate_asm(generator);

        FILE* a;
        if((a = fopen("a.out", "r"))){
            system("rm a.out");
            fclose(a);
        }

        system("as -o assem.o assem.s");
        char command[256];
        snprintf(command, sizeof(command), "ld -o %s assem.o", (output_path == NULL) ? "a.out" : output_path);
        system(command);
        system("rm assem.o");
        if(keep_assembly == false) system("rm assem.s");
    }
    generator_free(generator);
    lexer_free(lexer);
    list_free(vars);


    free(source);
}
