#ifndef AST_HEADER
#define AST_HEADER

//typedef struct ASTNode ASTNODE;

typedef struct ASTNODE{
    enum{
        NODE_LITERAL,
        NODE_VARIABLE,
        NODE_BINARY_OP,
        NODE_OPERATOR
    }type;

    int value;
    char* name;
    int op_type;
    struct ASTNODE* left;
    struct ASTNODE* right;
    struct ASTNODE* parent;

}ASTNode;

ASTNode* create_literal_node(int value);
ASTNode* create_variable_node(const char* name);
ASTNode* create_parent_node(int op_type);
ASTNode* create_binary_op_node(ASTNode* left, ASTNode* right, int op_type);


int node_get_value(ASTNode* node);

#endif
