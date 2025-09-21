#ifndef FUNCTION_HEADER
#define FUNCTION_HEADER

typedef struct FUNCTION_STRUCT {
    char* name;
    enum{
        FUNCTION_INT,
        FUNCTION_VOID,
        FUNCTION_STRING,
        FUNCTION_CHAR
    }type;
    char* block;
    int block_pos;
}function;



#endif
