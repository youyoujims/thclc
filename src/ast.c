#include "headers/ast.h"
#include <stdlib.h>
#include <string.h>


ASTNode* create_literal_node(int value){
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
    node->type = NODE_LITERAL;
    node->value = value;
    return node;
}

ASTNode* create_parent_node(int op_type){
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
    node->op_type = op_type;
    node->type = NODE_OPERATOR;
    return node;
}

ASTNode* create_binary_op_node(ASTNode* left, ASTNode* right, int op_type){
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
    node->type = NODE_BINARY_OP;
    node->op_type = op_type;
    node->left = left;
    node->right = right;
    node->parent = NULL;
    if(left) left->parent = node;
    if(right) right->parent = node;
    node->name = NULL;
    node->value = 0;
    return node;
}

ASTNode* create_variable_node(const char* name){
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
    node->type = NODE_VARIABLE;
    node->name = calloc(strlen(name)+1, sizeof(char));
    strcpy(node->name, name);
    return node;
}
