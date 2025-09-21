#include "headers/vars_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

vars_list* list_init(){
    vars_list* vars = calloc(1, sizeof(vars_list));
    vars->content = NULL;
    vars->len = 0;

    return vars;
}

int list_append(vars_list* vars, char* item){
    vars->len++;
    if(vars->content == NULL){
        vars->content = calloc(vars->len, sizeof(char*));
    }else{
        vars->content = realloc(vars->content, vars->len * sizeof(char*));
    }
    vars->content[vars->len-1] = strdup(item);

    return vars->len-1;
}

void list_print_list(vars_list* vars){
    for(int i = 0; i < vars->len; i++){
        if(vars->content[i] != NULL){
            printf("%s\n", vars->content[i]);
        }
    }
}

int list_contains(vars_list* vars, char* item){
    for(int i = 0; i < vars->len; i++){
        char* copy = strdup(vars->content[i]);
        if(copy == NULL){
             printf("Memory alloc failed\n");
             exit(1);
        }

        char* type = strtok(copy, " ");
        if(type != NULL){
            if(strcmp(type, item) == 0){
                free(copy);
                return 1;
            }
        }

        free(copy);
    }

    return 0;
}

char* list_get(vars_list* vars, char* item){
    for(int i = 0; i < vars->len; i++){
        char* copy = strdup(vars->content[i]);
        if(copy == NULL){
            printf("Memory alloc failed\n");
            exit(1);
        }

        //fix this 
        char* type = strtok(copy, " ");
        char* var_name = strtok(NULL, " ");
        if(type != NULL && var_name != NULL){
            if(strcmp(type, item) == 0){
                char* result = strdup(var_name);
                free(copy);
                return result;
            }
        }

        free(copy);
    }
    return NULL;
}

int list_type_equals_to(vars_list* vars, char* item_name, char* item_type){
    for(int i = 0; i < vars->len; i++){
        char* copy = strdup(vars->content[i]);
        if(copy == NULL){
            printf("Memory alloc failed\n");
            exit(1);
        }

        char* type = strtok(copy, " ");
        char* var_name = strtok(NULL, " ");
        if(type != NULL && var_name != NULL){
            if(strcmp(var_name, item_name) == 0){
                if(strncmp(type, item_type, strlen(item_type)) == 0){
                    free(copy);
                    return 1;
                }
            }
        }

        free(copy);
    }
    return 0;
}

int list_is_constant(vars_list* vars, char* item_name){
    for(int i = 0; i < vars->len; i++){
        char* copy = strdup(vars->content[i]);
        if(copy == NULL){
            printf("Memory alloc failed\n");
            exit(1);
        }

        char* type = strtok(copy, " ");
        char* var_name = strtok(NULL, " ");
        if(type != NULL && var_name != NULL){
            if(strcmp(var_name, item_name) == 0){
                if(strncmp(type, "const_", 6) == 0){
                    free(copy);
                    return 1;
                }
            }
        }

        free(copy);
    }
    return 0;
}

void list_free(vars_list* vars){
    for(int i = 0; i < vars->len; i++){
        free(vars->content[i]);
    }
    free(vars->content);
    free(vars);
}
