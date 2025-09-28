#include <stdio.h>
#include "headers/compiler.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define VERSION 1.0

static char* shift(int* argc, char*** argv){
    assert(*argc > 0);
    char* result = **argv;
    (*argv)++;
    (*argc)--;
    return result;
}		

void usage(){
    printf("Usage: thclc [OPTIONS] <file.hcl>\n");
    printf("OPTIONS:\n");
    printf("    -h, --help   \t\tShow this information\n");
	printf("    -v, --version\t\tDisplays the version of the compiler\n");
	printf("    --version    \t\tDisplays the version of the compiler\n");
    printf("    -o           \t\tDefine the output file name\n");
	printf("    -k           \t\tKeeps the assembly file\n");
}

int main(int argc, char** argv){
    bool keep_assembly = false;
    shift(&argc, &argv);
    char* input_file_path = NULL;
    char* output_file_path = NULL;
    if(argc < 1){
        printf("Usage: thclc [OPTIONS] <file.hcl>\n");
        exit(1);
    }
    while(argc > 0){
        char* flag = shift(&argc, &argv);
        if(strcmp(flag, "--help") == 0 || strcmp(flag, "-h") == 0){
            usage();
            exit(1);
        }else if(strcmp(flag, "--version") == 0 || strcmp(flag, "-v") == 0){
            printf("The Horrible Coding Language Compiler version %f\n", VERSION);
            exit(1);
        } else if(strcmp(flag, "-o") == 0){
            if(argc <= 0){
                usage();
                exit(1);
            }
            output_file_path = shift(&argc, &argv);
        }else if(strcmp(flag, "-k") == 0){
            keep_assembly = true;
        }else {
            input_file_path = flag;
        }
    }
    compiler_compile(input_file_path, output_file_path, keep_assembly);
    
    return 0;
}  
