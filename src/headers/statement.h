#ifndef STATEMENT_HEADER
#define STATEMENT_HEADER
#include "ast.h"
#include "function.h"
#include <stdbool.h>

typedef struct STATEMENT_STRUCT{
    function* function;
    enum{
        STATEMENT_REASSIGNEMENT,
        STATEMENT_CALL,
        STATEMENT_INITILISATION,
        STATEMENT_INITILISATION_CONST,
        STATEMENT_RETURN,
        STATEMENT_PRINT,
        STATEMENT_CONDITION
    }type;
    char* name;
    char* args;
    int token_type;
    bool has_expr;
    ASTNode* expr;
}statement;


#endif
