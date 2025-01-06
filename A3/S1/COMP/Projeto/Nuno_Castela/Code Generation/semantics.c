#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ast.h"
#include "semantics.h"

#define MAX_ERRORS 4096
#define MAX_ERROR_SIZE 4096
char *error_messages[MAX_ERRORS];
int error_count = 0;

int semantic_errors = 0;

struct symbol_list *symbol_table;
struct scopes_queue *symbol_scopes;


extern char *type_name[];
char *type_name_stat[] = t_name;

enum type category_to_type(enum category category){
    switch(category){
        case Int:
        case Natural:
            return integer;
        case Float32:
        case Decimal:
            return float32;
        case Bool:
            return bool;
        case String:
        case StrLit:
            return string;
        default:
            return undef;

    }
}

char *category_to_operator(enum category cat){
    switch(cat){
        case Or:
            return "||";
        case And:
            return "&&";
        case Eq:
            return "==";
        case Ne:
            return "!=";
        case Lt:
            return "<";
        case Gt:
            return ">";
        case Le:
            return "<=";
        case Ge:
            return ">=";
        case Add:
            return "+";
        case Sub:
            return "-";
        case Mul:
            return "*";
        case Div:
            return "/";
        case Mod:
            return "%";
        case Not:
            return "!";
        case Minus:
            return "-";
        case Plus:
            return "+";
        default:
            return "";
    }
}

void add_error(char *message) {
    error_messages[error_count++] = strdup(message);
}

int compare_errors(const void *a, const void *b) {
    char *error_a = *(char **)a;
    char *error_b = *(char **)b;
    return strcmp(error_a, error_b);
}

void sort_and_print_errors() {
    qsort(error_messages, error_count, sizeof(char *), compare_errors);
    for (int i = 0; i < error_count; i++) {
        printf("%s", error_messages[i]);
        free(error_messages[i]);
    }
}

int check_program(struct node *program){
    if(program == NULL){
        return 0;
    }

    // Initialize the global symbol table
    symbol_table = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    symbol_table->identifier = "global";
    symbol_table->next = NULL;

    // Initialize the local scopes
    symbol_scopes = NULL; // Initialize to NULL

    struct node_list *cur_child = program->children;

    while((cur_child = cur_child->next) != NULL){
        struct node *cur_node = cur_child->node;
        enum category cur_category = cur_node->category;
        if(cur_category == VarDecl){
            check_var_decl(cur_node, symbol_table);
        }
        if(cur_category == FuncDecl){
            check_func_decl(cur_node);
        }
    }

    // Start checking the function bodies
    struct scopes_queue *cur_scope = symbol_scopes;
    while (cur_scope != NULL) {
        check_func_body(cur_scope->func_body, cur_scope->table);
        cur_scope = cur_scope->next;
    }

    check_unused_symbols();
    sort_and_print_errors();
    return semantic_errors;
}

void check_func_decl(struct node *func_decl){
    if(func_decl == NULL){
        return;
    }

    struct node *func_header = get_child(func_decl, 0);
    struct node *id = get_child(func_header, 0);

    if(search_symbol(symbol_table, id->token) != NULL){
        semantic_errors++;
        char buffer[MAX_ERROR_SIZE];
        sprintf(buffer, "Line %d, column %d: Symbol %s already defined\n", id->token_line, id->token_column, id->token);
        add_error(buffer);
        return;
    }

    // Get the return type of the function
    struct node *return_type_node = get_child(func_header, 1);

    enum type return_type;
    struct node *func_params;
    // If the second child of the FuncHeader are the parameters, then there is no return type
    if(return_type_node->category == FuncParams){
        return_type = none;
        func_params = get_child(func_header, 1); // The parameters are the second child of the FuncHeader if there is no return type
    }
    else{
        return_type = category_to_type(return_type_node->category);
        func_params = get_child(func_header, 2); // The parameters are the third child of the FuncHeader if there is a return type
    }

    // Insert the function id in the global symbol table
    insert_symbol(symbol_table, id->token, return_type, func_decl, 0, 1, 1);


    // Create a new scope for the function
    struct symbol_list *new_scope = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    new_scope->identifier = strdup(id->token);
    new_scope->next = NULL;

    // Insert the return type in the new scope
    insert_symbol(new_scope, "return", return_type, NULL, 0, 1, 0);


    // Add the new scope to the queue
    struct scopes_queue *new_queue_entry = malloc(sizeof(struct scopes_queue));
    new_queue_entry->table = new_scope;
    new_queue_entry->next = NULL;
    new_queue_entry->identifier = strdup(id->token);

    if (symbol_scopes == NULL) {
        symbol_scopes = new_queue_entry;
    }
    else {
        struct scopes_queue *current = symbol_scopes;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_queue_entry;
    }

    // Check the function parameters
    char *param_types = check_parameters(func_params, new_scope);

    struct node *func_body = get_child(func_decl, 1);

    // Store the function body in the scope queue
    new_queue_entry->func_body = func_body;

    // Store the function parameters in the symbol table
    struct symbol_list *func_symbol = search_symbol(symbol_table, id->token);
    func_symbol->function_parameters = strdup(param_types);
}

// Return 1 if the parameters are correct, 0 otherwise
char* check_parameters(struct node *func_params, struct symbol_list *scope){
    if(func_params == NULL){
        return "";
    }


    int has_undefined = 0;
    struct node_list *cur_child = func_params->children;

    char *param_types = (char *)malloc(4096);
    while((cur_child = cur_child->next) != NULL){
        struct node *param_decl = cur_child->node;
        struct node *id = get_child(param_decl, 1);
        struct node *type = get_child(param_decl, 0);
        if(search_symbol(scope, id->token) != NULL){
            semantic_errors++;
            char buffer[MAX_ERROR_SIZE];
            sprintf(buffer, "Line %d, column %d: Symbol %s already defined\n", id->token_line, id->token_column, id->token);
            add_error(buffer);
            has_undefined++;
        }
        else{
            enum category type_category = type->category;
            enum type var_type = category_to_type(type_category);
            insert_symbol(scope, id->token, var_type, param_decl, 1, 1, 0);
        }
        strcat(param_types, type_name[category_to_type(type->category)]);
        strcat(param_types, ",");


    }

    // Remove the last comma
    int len = strlen(param_types);
    if (len > 0) {
        param_types[len - 1] = '\0';
    }

    return param_types;
}

void check_func_body(struct node *func_body, struct symbol_list *scope){
    if(func_body == NULL){
        return;
    }
    struct node_list *cur_child = func_body->children;
    while((cur_child = cur_child->next) != NULL){
        struct node *cur_node = cur_child->node;

        // There are only VarDecl and Statements in the function body
        if(cur_node->category == VarDecl){
            check_var_decl(cur_node, scope);
        }
        else{
            check_statements(cur_node, scope);
        }
    }
}

void check_statements(struct node *statement, struct symbol_list *scope){
    if(statement == NULL){
        return;
    }
    enum category cur_category = statement->category;
    type_name_stat[none] = "void";

    if(cur_category == Assign){
        check_assign(statement, scope);
    }
    if(cur_category == If){
        check_if(statement, scope);
    }
    if(cur_category == For){
        check_for(statement, scope);
    }
    if(cur_category == Return){
        check_return(statement, scope);
    }
    if(cur_category == Call){
        enum type call_type = check_call(statement, scope);
        statement->type = call_type;
    }
    if(cur_category == Block){
        struct node_list *cur_child = statement->children;
        while((cur_child = cur_child->next) != NULL){
            struct node *cur_node = cur_child->node;
            check_statements(cur_node, scope);
        }
    }
    if(cur_category == Print){
        struct node *right = get_child(statement, 0);
        enum type right_type = check_expression(right, scope);
        if(right_type == undef || right_type == none){
            semantic_errors++;
            char buffer[MAX_ERROR_SIZE];
            sprintf(buffer, "Line %d, column %d: Incompatible type %s in fmt.Println statement\n", right->token_line, right->token_column, type_name_stat[right_type]);
            add_error(buffer);
        }
    }
    // Right must be int and left must be int or string
    if(cur_category == ParseArgs){
        check_parse_args(statement, scope);
    }
}

void check_parse_args(struct node *parse_args, struct symbol_list *scope){
    if(parse_args == NULL){
        return;
    }
    struct node *right = get_child(parse_args, 0);
    struct node *left = get_child(parse_args, 1);

    enum type left_type = check_expression(left, scope);
    enum type right_type = check_expression(right, scope);

    if((right_type != integer) || (left_type != integer && left_type != string)){
        semantic_errors++;
        char buffer[MAX_ERROR_SIZE];
        sprintf(buffer, "Line %d, column %d: Operator strconv.Atoi cannot be applied to types %s, %s\n", parse_args->token_line, parse_args->token_column, type_name_stat[right_type], type_name_stat[left_type]);
        add_error(buffer);
    }

    // Annotate the AST
    parse_args->type = left_type;
}

// Checks function calls both as statements and as expressions
enum type check_call(struct node *call_node, struct symbol_list *scope){
    if(call_node == NULL){
        return undef;
    }

    struct node *id = get_child(call_node, 0);

    int correclty_called = 1;
    char *call_args_types = (char *)malloc(4096);
    call_args_types[0] = '\0';
    // Create a string with the types of the arguments of the function call
    struct node_list *cur_args_child = call_node->children->next;
    while((cur_args_child = cur_args_child->next) != NULL){
        struct node *arg = cur_args_child->node;
        enum type arg_type = check_expression(arg, scope);
        char *arg_type_str = type_name[arg_type];
        strcat(call_args_types, arg_type_str);
        strcat(call_args_types, ",");
    }

    // Remove the last comma
    int len = strlen(call_args_types);
    if (len > 0) {
        call_args_types[len - 1] = '\0';
    }

    // Get the original function's parameters
    char *func_params_types = get_func_parameter_types(id->token);
    enum type return_type;

    // Check if the function exists
    struct symbol_list *func_symbol = search_symbol(symbol_table, id->token);
    if(func_symbol == NULL){
        semantic_errors++;
        char buffer[MAX_ERROR_SIZE];
        sprintf(buffer, "Line %d, column %d: Cannot find symbol %s(%s)\n", id->token_line, id->token_column, id->token, call_args_types);
        add_error(buffer);
        correclty_called = 0;
        return_type = undef;
    }
    else{
        // Check if the function's parameters match the call's arguments
        if((func_symbol->is_function == 0) || (strcmp(func_params_types, call_args_types) != 0)){
            semantic_errors++;
            char buffer[MAX_ERROR_SIZE];
            sprintf(buffer, "Line %d, column %d: Cannot find symbol %s(%s)\n", id->token_line, id->token_column, id->token, call_args_types);
            add_error(buffer);
            correclty_called = 0;
            return_type = undef;
        }
        // The function was correctly called
        else{
            return_type = func_symbol->type;
        }
    }

    // Annotate the AST
    if(correclty_called){
        id->parameter_list = strdup(call_args_types);
    }
    else{
        id->parameter_list = strdup("undef");
    }

    return return_type;
}

void check_return(struct node *return_node, struct symbol_list *scope){
    if(return_node == NULL){
        return;
    }

    struct node *return_expr = get_child(return_node, 0);
    enum type return_type;
    enum type declared_return_type = search_symbol(scope, "return")->type;
    if(return_expr == NULL){
        return_type = none;

        if(declared_return_type != none){
            semantic_errors++;
            char buffer[MAX_ERROR_SIZE];
            sprintf(buffer, "Line %d, column %d: Incompatible type void in return statement\n", return_node->token_line, return_node->token_column);
            add_error(buffer);
        }

    }
    else{
        return_type = check_expression(return_expr, scope);


        if(return_type != declared_return_type){
            semantic_errors++;
            char buffer[MAX_ERROR_SIZE];
            sprintf(buffer, "Line %d, column %d: Incompatible type %s in return statement\n", return_expr->token_line, return_expr->token_column, type_name_stat[return_type]);
            add_error(buffer);
        }

    }
}

// Check the for statement
void check_for(struct node *for_node, struct symbol_list *scope){
    if(for_node == NULL){
        return;
    }

    struct node *for_expr = get_child(for_node, 0);
    struct node *for_block;
    if(for_expr->category == Block){
        for_block = for_expr;
        for_expr = NULL;
    }
    else{
        for_block = get_child(for_node, 1);
    }

    enum type expr_type = none;

    if(for_expr != NULL){
        expr_type = check_expression(for_expr, scope);
    }

    if((expr_type != bool && expr_type != none) || expr_type == undef){
        semantic_errors++;
        char buffer[MAX_ERROR_SIZE];
        sprintf(buffer, "Line %d, column %d: Incompatible type %s in for statement\n", for_expr->token_line, for_expr->token_column, type_name_stat[expr_type]);
        add_error(buffer);
    }

    if(for_block != NULL){
        check_statements(for_block, scope);
    }

}

// Check the if statement
void check_if(struct node *if_node, struct symbol_list *scope){
    if(if_node == NULL){
        return;
    }

    struct node *condition_expr = get_child(if_node, 0);
    struct node *if_block = get_child(if_node, 1);
    struct node *else_block = get_child(if_node, 2);

    // Check if the condition expression is a boolean
    enum type expr_type = check_expression(condition_expr, scope);
    if(expr_type != bool){
        semantic_errors++;
        char buffer[MAX_ERROR_SIZE];
        sprintf(buffer, "Line %d, column %d: Incompatible type %s in if statement\n", condition_expr->token_line, condition_expr->token_column, type_name_stat[expr_type]);
        add_error(buffer);
    }


    if(if_block != NULL){
        check_statements(if_block, scope);
    }
    if(else_block != NULL){
        check_statements(else_block, scope);
    }
}

// Check the assign statement
void check_assign(struct node *assign, struct symbol_list *scope){
    if(assign == NULL){
        return;
    }
    struct node *left = get_child(assign, 0);
    struct node *right = get_child(assign, 1);

    enum type left_type;
    // Check if the variable exists
    if(!var_exists(left, scope)){
        left_type = undef;
    }
    else{
        struct symbol_list *symbol;

        // If it's in the current scope, use it's scope type
        if(search_symbol(scope, left->token) != NULL){
            symbol = search_symbol(scope, left->token);
        }
        // If it's not in the current scope, use it's global scope type
        else{
            symbol = search_symbol(symbol_table, left->token);
        }

        // Annotate the AST
        left_type = symbol->type;

        // Mark symbol as used
        symbol->was_used = 1;
    }

    enum type right_type = check_expression(right, scope);
    right->type = right_type;
    left->type = left_type;

    if((left_type != right_type) || (left_type == undef) || (right_type == undef) || (right_type == none) || (left_type == none)){
        semantic_errors++;
        char buffer[MAX_ERROR_SIZE];
        sprintf(buffer, "Line %d, column %d: Operator = cannot be applied to types %s, %s\n", assign->token_line, assign->token_column, type_name_stat[left_type], type_name_stat[right_type]);
        add_error(buffer);
    }

    // Annotate the AST
    assign->type = left_type;
}

enum type check_expression(struct node *expression, struct symbol_list *scope){
    if(expression == NULL){
        return undef;
    }

    enum category cat = expression->category;
    enum type expr_type;

    // If the expression can't be matched
    expr_type = none;
    if(cat == Identifier){
        if(var_exists(expression, scope)){
            struct symbol_list *symbol;

            // If it's in the current scope, use it's scope type
            if((symbol = search_symbol(scope, expression->token)) != NULL){
                expr_type = symbol->type;
            }
            // If it's not in the current scope, use it's global scope type
            else{
                symbol = search_symbol(symbol_table, expression->token);
                expr_type = symbol->type;
            }

            // Mark symbol as used
            symbol->was_used = 1;
        }
        else{
            expr_type = undef;
        }
        expression->type = expr_type;

        return expr_type;
    }
    if(cat == Natural || cat == Decimal){
        expr_type = category_to_type(cat);
        expression->type = expr_type;
        return expr_type;
    }
    if(cat == StrLit){
        expr_type = string;
        expression->type = expr_type;
        return expr_type;
    }

    char *type_name_expr[] = t_name;
    type_name_expr[none] = "void";
    // Applies to integers, floats, strings and booleans
    if(cat == Eq || cat == Ne){
        struct node *left = get_child(expression, 0);
        struct node *right = get_child(expression, 1);
        enum type left_type = check_expression(left, scope);
        enum type right_type = check_expression(right, scope);
        if((left_type != right_type) || !(left_type == integer || left_type == float32 || left_type == string || left_type == bool)){
            semantic_errors++;
            char buffer[MAX_ERROR_SIZE];
            sprintf(buffer, "Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",
                   expression->token_line, expression->token_column,
                   category_to_operator(expression->category),
                   type_name_expr[left_type], type_name_expr[right_type]);
            add_error(buffer);
        }

        expr_type = bool;
        expression->type = expr_type;
        return expr_type;
    }
    // Only applies to integers and floats and always returns bool
    if(cat == Lt || cat == Gt || cat == Le || cat == Ge){
        struct node *left = get_child(expression, 0);
        struct node *right = get_child(expression, 1);
        enum type left_type = check_expression(left, scope);
        enum type right_type = check_expression(right, scope);
        if(left_type != right_type || !(left_type == integer || left_type == float32)){

            semantic_errors++;
            char buffer[MAX_ERROR_SIZE];
            sprintf(buffer, "Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",
                   expression->token_line, expression->token_column,
                   category_to_operator(expression->category),
                   type_name_expr[left_type], type_name_expr[right_type]);
            add_error(buffer);
        }

        expr_type = bool;
        expression->type = expr_type;
        return expr_type;
    }
    // Only applies to booleans
    if(cat == Or || cat == And){
        struct node *left = get_child(expression, 0);
        struct node *right = get_child(expression, 1);
        enum type left_type = check_expression(left, scope);
        enum type right_type = check_expression(right, scope);
        if(left_type != bool || right_type != bool){
            semantic_errors++;
            char buffer[MAX_ERROR_SIZE];
            sprintf(buffer, "Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",
                   expression->token_line, expression->token_column,
                   category_to_operator(expression->category),
                   type_name_expr[left_type], type_name_expr[right_type]);
            add_error(buffer);
        }

        expr_type = bool;
        expression->type = expr_type;
        return expr_type;
    }
    // Applies to integers and floats
    if(cat == Add || cat == Sub || cat == Mul || cat == Div || cat == Mod){
        struct node *left = get_child(expression, 0);
        struct node *right = get_child(expression, 1);
        enum type left_type = check_expression(left, scope);
        enum type right_type = check_expression(right, scope);
        if((left_type != right_type) || !(left_type == integer || left_type == float32)){
            semantic_errors++;
            char buffer[MAX_ERROR_SIZE];
            sprintf(buffer, "Line %d, column %d: Operator %s cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, category_to_operator(expression->category), type_name_expr[left_type], type_name_expr[right_type]);
            add_error(buffer);
            expr_type = undef;
        }
        else{
            expr_type = left_type;
        }
        expression->type = expr_type;
        return expr_type;
    }
    if(cat == Call){
        expr_type = check_call(expression, scope);
        expression->type = expr_type;
        return expr_type;
    }
    // Only applies to integers and floats
    if(cat == Minus || cat == Plus){
        struct node *right = get_child(expression, 0);
        enum type right_type = check_expression(right, scope);
        if(right_type != integer && right_type != float32){
            semantic_errors++;
            char buffer[MAX_ERROR_SIZE];
            sprintf(buffer, "Line %d, column %d: Operator %s cannot be applied to type %s\n", expression->token_line, expression->token_column, category_to_operator(expression->category), type_name_expr[right_type]);
            add_error(buffer);
            expr_type = undef;
        }
        else{
            expr_type = right_type;
        }
        expression->type = expr_type;
        return expr_type;
    }
    // Only applies to boolean
    if(cat == Not){
        struct node *right = get_child(expression, 0);
        enum type right_type = check_expression(right, scope);
        if(right_type != bool){
            semantic_errors++;
            char buffer[MAX_ERROR_SIZE];
            sprintf(buffer, "Line %d, column %d: Operator ! cannot be applied to type %s\n", expression->token_line, expression->token_column, type_name_expr[right_type]);
            add_error(buffer);
        }
        expr_type = bool;
        expression->type = expr_type;
        return expr_type;
    }

    expression->type = expr_type;
    return expr_type;
}

int var_exists(struct node *var, struct symbol_list *scope){
    // It does not exist in the current scope and it does not exist in the global scope or it exists in the global scope but it is a function
    if(
            ((search_symbol(scope, var->token) == NULL) && (search_symbol(symbol_table, var->token) == NULL))
        ||  ((search_symbol(scope, var->token) == NULL) && ((search_symbol(symbol_table, var->token) != NULL && search_symbol(symbol_table, var->token)->is_function)))
    )
    {
        semantic_errors++;
        char buffer[MAX_ERROR_SIZE];
        sprintf(buffer, "Line %d, column %d: Cannot find symbol %s\n", var->token_line, var->token_column, var->token);
        add_error(buffer);
        return 0;
    }

    return 1;
}

int func_exists(struct node *func){
    if((search_symbol(symbol_table, func->token) == NULL) || (search_symbol(symbol_table, func->token)->is_function == 0)){
        semantic_errors++;
        char buffer[MAX_ERROR_SIZE];
        sprintf(buffer, "Line %d, column %d: Cannot find symbol %s\n", func->token_line, func->token_column, func->token);
        add_error(buffer);
        return 0;
    }
    return 1;
}

void check_var_decl(struct node *var_decl, struct symbol_list *scope){
    if(var_decl == NULL){
        return;
    }

    // The id is the second child of the VarDecl node
    struct node *id = get_child(var_decl, 1);
    // The type is the first child of the VarDecl node
    struct node *type = get_child(var_decl, 0);

    // Check if the variable is already declared in the current scope
    if(search_symbol(scope, id->token) != NULL){
        semantic_errors++;
        char buffer[MAX_ERROR_SIZE];
        sprintf(buffer, "Line %d, column %d: Symbol %s already defined\n", id->token_line, id->token_column, id->token);
        add_error(buffer);
        return;
    }


    // Insert the variable in the current scope
    enum category type_category = type->category;
    enum type var_type = category_to_type(type_category);
    insert_symbol(scope, id->token, var_type, var_decl, 0, 0, 0);
}

// insert a new symbol in the list, unless it is already there
struct symbol_list *insert_symbol(struct symbol_list *table, char *identifier, enum type type, struct node *node, int is_parameter, int mark_as_used, int is_function) {
    struct symbol_list *new = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    new->identifier = strdup(identifier);
    new->type = type;
    new->node = node;
    new->next = NULL;
    new->is_parameter = is_parameter;
    new->was_used = mark_as_used;
    new->is_function = is_function;
    new->function_parameters = NULL;
    if(is_function){
        new->function_parameters = (char *)malloc(4096);
    }

    struct symbol_list *symbol = table;
    while(symbol != NULL) {
        if(symbol->next == NULL) {
            symbol->next = new;    // insert new symbol at the tail of the list
            break;
        } else if(strcmp(symbol->next->identifier, identifier) == 0) {
            free(new);
            return NULL;           // return NULL if symbol is already inserted
        }
        symbol = symbol->next;
    }
    return new;
}

// look up a symbol by its identifier
struct symbol_list *search_symbol(struct symbol_list *table, char *identifier) {
    struct symbol_list *symbol;
    //printf("identifier: %s\n", identifier);
    for(symbol = table->next; symbol != NULL; symbol = symbol->next)
        if(strcmp(symbol->identifier, identifier) == 0)
            return symbol;
    return NULL;
}

char *get_func_parameter_types(char *function_name) {
    struct symbol_list *symbol = search_symbol(symbol_table, function_name);
    if(symbol == NULL){
        return "";
    }
    if(symbol->function_parameters == NULL){
        return "";
    }
    char *result = (char *)malloc(4096);
    strcpy(result, symbol->function_parameters);
    return result;
}

void show_symbol_table() {
    printf("===== Global Symbol Table =====\n");
    struct symbol_list *symbol;
    for(symbol = symbol_table->next; symbol != NULL; symbol = symbol->next){
        if(symbol->node->category == FuncDecl){
            printf("%s\t(%s)\t%s\n", symbol->identifier, get_func_parameter_types(symbol->identifier), type_name[symbol->type]);
        }
        if(symbol->node->category == VarDecl){
            printf("%s\t\t%s\n", symbol->identifier, type_name[symbol->type]);
        }
    }
    printf("\n");

}

void show_symbol_scopes() {
    // Print the scopes in FIFO order
    struct scopes_queue *cur_scope = symbol_scopes;
    while (cur_scope != NULL) {
        printf("===== Function %s(%s) Symbol Table =====\n",
               cur_scope->identifier, get_func_parameter_types(cur_scope->identifier));
        struct symbol_list *cur_symbol = cur_scope->table;
        while ((cur_symbol = cur_symbol->next) != NULL) {
            printf("%s\t\t%s", cur_symbol->identifier, type_name[cur_symbol->type]);
            if (cur_symbol->is_parameter) {
                printf("\tparam");
            }
            printf("\n");
        }
        printf("\n");
        cur_scope = cur_scope->next;
    }
}

// Function to print unused symbols
void check_unused_symbols() {
    struct symbol_list *symbol;
    // Global variables can be unused

    struct scopes_queue *cur_scope = symbol_scopes;
    while (cur_scope != NULL) {
        symbol = cur_scope->table->next;
        while (symbol != NULL) {
            if(symbol->was_used == 0 && !symbol->is_parameter){
                struct node *node = symbol->node;
                struct node *id = get_child(node, 1);
                char buffer[MAX_ERROR_SIZE];
                sprintf(buffer, "Line %d, column %d: Symbol %s declared but never used\n", id->token_line, id->token_column, id->token);
                add_error(buffer);
            }
            symbol = symbol->next;
        }
        cur_scope = cur_scope->next;
    }
}

void free_symbol_table(struct symbol_list *table) {
    struct symbol_list *symbol = table;
    while(symbol != NULL) {
        struct symbol_list *next = symbol->next;
        free(symbol);
        symbol = next;
    }
}

void free_symbol_scopes() {
    struct scopes_queue *cur_scope = symbol_scopes;
    while (cur_scope != NULL) {
        struct scopes_queue *next = cur_scope->next;
        free_symbol_table(cur_scope->table);
        free(cur_scope->identifier);
        free(cur_scope);
        cur_scope = next;
    }
}
