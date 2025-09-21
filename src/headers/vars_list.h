#ifndef VARS_LIST_HEADER
#define VARS_LIST_HEADER

typedef struct VAR_LIST_STRUCT{
    char** content;
    int len;
}vars_list;

vars_list* list_init();
 
char* list_get_at(vars_list* vars, int index);

int list_append(vars_list* vars, char* item);

int list_contains(vars_list* vars, char* item);

char* list_get(vars_list* vars, char* item);

int list_is_constant(vars_list* vars, char* item_name);

int list_type_equals_to(vars_list* vars, char* item_name, char* item_type);

void list_print_list(vars_list* vars);

void list_free(vars_list* vars);

#endif
