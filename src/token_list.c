#include "headers/token_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

token_list* list_token_init(){
    token_list* list = calloc(1, sizeof(token_list));
    list->content = NULL;
    list->len = 0;

    return list;
}

int list_token_append(token_list* list, token* item){
    list->len++;
    if(list->content == NULL){
        list->content = calloc(list->len, sizeof(token_list));
    }else{
        list->content = realloc(list->content, list->len * sizeof(token_list));
    }
    token* token = token_init(item->value, item->type);
    list->content[list->len-1] = token;

    return list->len-1;
}

void list_token_print_list(token_list* list){
    for(int i = 0; i < list->len; i++){
        if(list->content[i] != NULL){
            printf("%s\n", list->content[i]->value);
        }
    }
}

void list_token_free(token_list* list){
    for(int i = 0; i < list->len; i++){
        token_free(list->content[i]);
    }
    free(list->content);
    free(list);
}
