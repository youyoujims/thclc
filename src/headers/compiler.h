#ifndef COMPILER_HEADER
#define COMPILER_HEADER
#include "generator.h"

void compiler_compile(char* file_path, char* output_path, bool keep_assembly);
char* compiler_read_file(char* file_path);


#endif
