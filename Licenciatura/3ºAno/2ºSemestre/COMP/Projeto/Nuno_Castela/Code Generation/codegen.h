#ifndef _CODEGEN_H
#define _CODEGEN_H

#include "ast.h"
#include "semantics.h"

char* llvm_types(enum type type);
void codegen_program(struct node *program);
void codegen_function(struct node * function);
void codegen_func_header(struct node *func_header, enum type return_type);
int codegen_statement(struct node *statement, int *has_returned_basic_block, int is_tail);
int codegen_expression(struct node *expression);
void codegen_body(struct node *func_body, int *has_returned_basic_block);
int codegen_natural(struct node *natural_node);
int codegen_identifier(struct node *id);
void codegen_print(struct node *print_node);
void codegen_assign(struct node *assign);
int codegen_add(struct node *add);  
int codegen_sub(struct node *sub);
int codegen_mul(struct node *mul);
int codegen_div(struct node *div);
int codegen_mod(struct node *mod);
int codegen_strlit(struct node *strlit);
int codegen_or(struct node *or);
int codegen_and(struct node *and_node);
int codegen_not(struct node *not);
int codegen_minus(struct node *minus);
int codegen_plus(struct node *plus);
int codegen_decimal(struct node *decimal);
int codegen_ne(struct node *ne);
int codegen_lt(struct node *lt);
int codegen_gt(struct node *gt);
int codegen_le(struct node *le);
int codegen_ge(struct node *ge);
int codegen_eq(struct node *eq);
int codegen_call(struct node *call, int is_expr, int is_tail);
void codegen_return(struct node *return_node);
void codegen_if(struct node *if_node, int *has_returned_basic_block);
void codegen_block(struct node *block, int *has_returned_basic_block);
void codegen_for(struct node *for_node, int *has_returned_basic_block);
void codegen_parse_args(struct node *parse_args);

#endif