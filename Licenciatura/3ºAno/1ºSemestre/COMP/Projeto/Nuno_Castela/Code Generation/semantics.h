#ifndef _SEMANTICS_H
#define _SEMANTICS_H

#include "ast.h"

#define MAX_ERRORS 4096
extern char *error_messages[MAX_ERRORS];
extern int error_count;

void add_error(char *message);
void sort_and_print_errors();

struct symbol_list {
    char *identifier;
    enum type type;
    struct node *node;
    int is_parameter;
    struct symbol_list *next;
    int was_used;
    int is_function;
    char *function_parameters;
};

// Sequence of symbol lists
struct scopes_queue {
    struct symbol_list *table;
    struct scopes_queue *next;
    struct node *func_body; // The function body node is stored here so it can be processed after the function declarations
    char *identifier;
};

enum type category_to_type(enum category category);

// Global variables
extern int program_errors;
extern struct node *current_function;
extern int inside_loop;
extern struct symbol_list *global_table;
extern struct scopes_queue *scopes;

void add_error(char *message);
int compare_errors(const void *a, const void *b);
void sort_and_print_errors();

int check_program(struct node *program);
void check_var_decl(struct node *var_decl, struct symbol_list *scope);

void check_func_decl(struct node *func_decl);
char* check_parameters(struct node *func_params, struct symbol_list *scope);
void check_func_body(struct node *func_body, struct symbol_list *scope);
void check_parse_args(struct node *parse_args, struct symbol_list *scope);

void check_statements(struct node *cur_node, struct symbol_list *scope);
enum type check_call(struct node *call_node, struct symbol_list *scope);
void check_assign(struct node *assign, struct symbol_list *scope);
void check_if(struct node *if_node, struct symbol_list *scope);
void check_for(struct node *for_node, struct symbol_list *scope);
void check_return(struct node *return_node, struct symbol_list *scope);
enum type check_expression(struct node *expression, struct symbol_list *scope);

void check_unused_symbols();
int var_exists(struct node *var, struct symbol_list *scope);
int func_exists(struct node *func);

struct symbol_list *insert_symbol(struct symbol_list *symbol_table, char *identifier, enum type type, struct node *node, int is_parameter, int mark_as_used, int is_function);
struct symbol_list *search_symbol(struct symbol_list *symbol_table, char *identifier);
char *get_func_parameter_types(char *function_name);
void show_symbol_table();
void show_symbol_scopes();
void free_symbol_table(struct symbol_list *symbol_table);
void free_symbol_scopes();


#endif
