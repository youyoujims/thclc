#ifndef TOKEN_LIST_HEADER
#define TOKEN_LIST_HEADER
#include "token.h"
typedef struct TOKEN_LIST_STRUCT{
    token** content;
    int len;
}token_list;

token_list* list_token_init();
 
token* list_token_get_at(token_list* list, int index);

int list_token_append(token_list* list, token* item);

int list_token_contains(token_list* list, token* item);

int list_token_is_constant(token_list* list, token* item_name);

void list_token_print_list(token_list* list);

void list_token_free(token_list* list);

#endif
