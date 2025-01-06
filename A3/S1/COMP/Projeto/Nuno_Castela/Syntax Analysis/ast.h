#ifndef _AST_H
#define _AST_H

enum category {   AUX ,  Program ,  VarDecl ,  FuncDecl ,  FuncHeader ,  FuncParams ,  FuncBody ,  ParamDecl ,  Block ,  If ,  For ,  Return ,  Call ,  Print ,  ParseArgs ,  Or ,  And ,  Eq ,  Ne ,  Lt ,  Gt ,  Le ,  Ge ,  Add ,  Sub ,  Mul ,  Div ,  Mod ,  Not ,  Minus ,  Plus ,  Assign ,  Int ,  Float32 ,  Bool ,  String ,  Natural ,  Decimal ,  Identifier ,  StrLit  };
#define names {  "AUX", "Program", "VarDecl", "FuncDecl", "FuncHeader", "FuncParams", "FuncBody", "ParamDecl", "Block", "If", "For", "Return", "Call", "Print", "ParseArgs", "Or", "And", "Eq", "Ne", "Lt", "Gt", "Le", "Ge", "Add", "Sub", "Mul", "Div", "Mod", "Not", "Minus", "Plus", "Assign", "Int", "Float32", "Bool", "String", "Natural", "Decimal", "Identifier", "StrLit" }


struct node {
    enum category category;
    char *token;
    struct node_list *children;
};

struct node_list {
    struct node *node;
    struct node_list *next;
};

struct node *new_node(enum category category, char *token);
void add_child(struct node *parent, struct node *child);
void remove_first_child(struct node *parent);
void dfs(struct node *cur_node, int depth);
void remove_aux(struct node *cur_node);
void free_ast(struct node *cur_node);
int block_elements(struct node* cur_node);

#endif
