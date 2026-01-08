%{
/*
    Authors:
        Nuno Batista: uc2022216127
        Miguel Castela: uc2022212972
*/
#include "ast.h"
#include <stdlib.h>
#include <string.h>

int yylex(void);
void yyerror(char*);

struct node *program;

%}

%debug

%union{
    char *lexeme;
    struct node *node;
}

%locations
%{
#define LOCATE(node, line, column) { node->token_line = line; node->token_column = column;}
%}

// Terminal symbols without associated values
%token              SEMICOLON COMMA STAR DIV MINUS PLUS EQ GE GT LBRACE LE LPAR LSQ LT 
                    MOD NE NOT AND OR PACKAGE ELSE FOR IF VAR INT FLOAT32 BOOL STRING PRINT PARSEINT 
                    FUNC CMDARGS BLANKID RBRACE RPAR RETURN RSQ ASSIGN 

// Terminal symbols with associated values
%token  <lexeme>    STRLIT RESERVED IDENTIFIER NATURAL DECIMAL

// Non-terminal symbols
%type   <node>      Program VarDecl FuncDecl FuncParams FuncBody 
//FuncHeader ParamDecl


// Aux Non-terminal symbols 
%type  <node>       Declarations VarSpec Type OptFuncParams OptType
                    VarsAndStatements Statement Expr OptElse StarStatementSc
                    OptExpr FuncInvocation ParseArgs PosExpr StarCommaId

%left       LOW
%nonassoc   IF ELSE FOR
%left       COMMA
%right      ASSIGN
%left       OR
%left       AND
%left       LT GT LE GE EQ NE
%left       PLUS MINUS
%left       STAR DIV MOD
%left       LBRACE RBRACE LPAR RPAR LSQ RSQ
%right      NOT 


%%

/* 
....Program(>=0)
........{VarDecl/FuncDecl}
*/
Program             :   PACKAGE IDENTIFIER SEMICOLON Declarations 
                        {
                            $$ = program = new_node(Program, NULL); 
                            add_child($$, $4);
                        }
                    ;

Declarations        :   Declarations FuncDecl SEMICOLON
                        {
                            $$ = $1;
                            add_child($$, $2);
                        }              
                    |   Declarations VarDecl SEMICOLON
                        {
                            $$ = $1;

                            struct node *var_declarations = new_node(AUX, NULL);
                            add_child($$, var_declarations);
                            add_child(var_declarations, $2);

                        }
                    |   
                        {
                            $$ = new_node(AUX, NULL);
                        }
                    ;


/*
....VarDecl(1)
........<typespec>
........Identifier
*/
VarDecl             :   VAR VarSpec                                             
                        {
                            $$ = $2;
                        }
                    |   VAR LPAR VarSpec SEMICOLON RPAR                         
                        {
                            $$ = $3;
                        }
                    ;   

VarSpec             :   IDENTIFIER StarCommaId Type
                        {
                            $$ = new_node(AUX, NULL);
                            struct node *new_decl = new_node(VarDecl, NULL);
                            add_child(new_decl, $3);
                            add_child(new_decl, new_node(Identifier, $1));
                            add_child($$, new_decl);
                            add_child($$, $2);
                            
                            LOCATE(get_child(new_decl, 1), @1.first_line, @1.first_column);

                            enum category type = $3->category;
                            // Pass down the type to the extra variable declarations
                            if($2 != NULL){
                                // First extra variable declaration ($2 is the AUX node)
                                struct node_list *cur_var_decl = $2->children->next;
                                while(cur_var_decl != NULL){
                                    struct node *type_node = new_node(type, NULL);

                                    /*
                                        If we just add the type node, the structure becomes as follows:
                                        
                                        VarDecl
                                        ..Id
                                        ..Type
                                        
                                        We want it to be:
                                        
                                        VarDecl
                                        ..Type
                                        ..Id
                                    */

                                    // Save identifier name
                                    struct node *var_decl_node = cur_var_decl->node; // VarDecl node
                                    struct node *id_node = var_decl_node->children->next->node; // Identifier node
                                    char *var_name = strdup(id_node->token); // Identifier name
                                    
                                    /*
                                        Add the type node before the adding the identifier node again
                                        The structure becomes as follows:

                                        VarDecl
                                        ..Id
                                        ..Type
                                        ..Id
                                    */
                                    add_child(cur_var_decl->node, type_node);

                                    struct node *new_id_node = new_node(Identifier, var_name);

                                    // Copy the line and column from the original identifier node
                                    new_id_node->token_line = id_node->token_line;
                                    new_id_node->token_column = id_node->token_column;
                                    add_child(cur_var_decl->node, new_id_node);

                                    /*
                                        Remove the first node from the VarDecl node
                                    */
                                    remove_first_child(cur_var_decl->node);                                    
                                    
                                    cur_var_decl = cur_var_decl->next;
                                }
                            }            
                        }

                    ;

StarCommaId         :   StarCommaId COMMA IDENTIFIER
                        {
                            $$ = $1;
                            struct node *new_decl = new_node(VarDecl, NULL);
                            add_child(new_decl, new_node(Identifier, $3));
                            add_child($$, new_decl);

                            LOCATE(get_child(new_decl, 0), @3.first_line, @3.first_column);
                            // The type is added in the VarSpec rule
                        }
                    |   
                        {
                            $$ = new_node(AUX, NULL);
                        }
                    ;

/*
LEAF NODES: INT, FLOAT32, BOOL, STRING
*/
Type                :   INT                                                     
                        {
                            $$ = new_node(Int, NULL);
                        }
                    |   FLOAT32                                                 
                        {
                            $$ = new_node(Float32, NULL);    
                        }
                    |   BOOL                                                    
                        {
                            $$ = new_node(Bool, NULL);
                        }
                    |   STRING                                                  
                        {
                            $$ = new_node(String, NULL);
                        }
                    ;

/* 
....FuncDecl(2) 
........FuncHeader(>=2)
............Identifier
............<typespec>
............FuncParams
................|SPECIFIED BELOW|
........FuncBody(>=0)
............|SPECIFIED BELOW|
*/
FuncDecl            :   FUNC IDENTIFIER LPAR OptFuncParams RPAR OptType FuncBody
                        {
                            $$ = new_node(FuncDecl, NULL);
                            
                            struct node *new_func_header = new_node(FuncHeader, NULL);
                            add_child($$, new_func_header);
                            add_child(new_func_header, new_node(Identifier, $2));
                            add_child(new_func_header, $6);
                            add_child(new_func_header, $4);

                            add_child($$, $7);

                            LOCATE(get_child(new_func_header, 0), @2.first_line, @2.first_column);
                        }
                    ;

OptFuncParams       :   FuncParams                           
                        {
                            $$ = $1;
                        }                        
                    |   
                        {
                            $$ = new_node(FuncParams, NULL);
                        }                                                        
                    ; 

OptType             :   Type                                                    
                        {
                            $$ = $1;
                        }
                    |   
                        {
                            $$ = new_node(AUX, NULL);
                        }
                    ;

/*
....FuncParams(>=0)
........ParamDecl
*/
FuncParams          :   IDENTIFIER Type                                         
                        {
                            $$ = new_node(FuncParams, NULL);

                            struct node *new_param_decl = new_node(ParamDecl, NULL);
                            add_child($$, new_param_decl);
                            add_child(new_param_decl, $2);
                            add_child(new_param_decl, new_node(Identifier, $1));

                            LOCATE(get_child(new_param_decl, 1), @1.first_line, @1.first_column);
                        }
                    |   FuncParams COMMA IDENTIFIER Type
                        {
                            $$ = $1;
                            
                            struct node *new_param_decl = new_node(ParamDecl, NULL);
                            add_child($$, new_param_decl);
                            add_child(new_param_decl, $4);  
                            add_child(new_param_decl, new_node(Identifier, $3));

                            LOCATE(get_child(new_param_decl, 1), @3.first_line, @3.first_column);
                        }
                    ;   

/*
....FuncBody(>=0)
........<declarations>/<statements>
*/
FuncBody            :   LBRACE VarsAndStatements RBRACE                         
                        {
                            $$ = new_node(FuncBody, NULL);
                            add_child($$, $2);
                        }
                    ;

VarsAndStatements   :   VarsAndStatements SEMICOLON                             
                        {
                            $$ = $1;
                        }
                    |   VarsAndStatements VarDecl SEMICOLON                     
                        {
                            $$ = $1;
                            add_child($$, $2);
                        }
                    |   VarsAndStatements Statement SEMICOLON                   
                        {
                            $$ = $1;
                            add_child($$, $2);
                        }
                    |                                                           
                        {
                            $$ = new_node(AUX, NULL);
                        } 
                    ;

/*
....Block(>=0) <- Only needed if there is more than 1 statement in the block

....If(3)

....For
........[Expr]
........Block

....Return(>=0)

....Call(>=1)

....Print(1)

....ParseArgs(2)

....Or(2) And(2) Eq(2) Ne(2) Lt(2)
....Gt(2) Le(2) Ge(2) Add(2) Sub(2) 
....Mul(2) Div(2) Mod(2) Not(1) Minus(1) 
....Plus(1) Assign(2) 

LEAF NODES: Natural, Decimal, Identifier, Strlit
*/
Statement           :   IDENTIFIER ASSIGN Expr                                  
                        {
                            $$ = new_node(Assign, NULL);
                            add_child($$, new_node(Identifier, $1));
                            add_child($$, $3);

                            LOCATE(get_child($$, 0), @1.first_line, @1.first_column);
                            LOCATE($$, @2.first_line, @2.first_column);
                        }
                    |   LBRACE StarStatementSc RBRACE                           
                        {
                            if(block_elements($2) > 1){
                                $$ = new_node(Block, NULL);
                                add_child($$, $2);
                            }
                            else{
                                $$ = $2;
                            }
                        }
                    |   IF Expr LBRACE StarStatementSc RBRACE OptElse %prec LOW 
                        {
                            $$ = new_node(If, NULL);
                            struct node* if_block = new_node(Block, NULL);
                            struct node* else_block = new_node(Block, NULL);
                            add_child($$, $2);

                            add_child($$, if_block);
                            add_child(if_block, $4);

                            add_child($$, else_block);
                            add_child(else_block, $6);
                        }
                    |   FOR OptExpr LBRACE StarStatementSc RBRACE               
                        {
                            $$ = new_node(For, NULL);
                            add_child($$, $2);
                            struct node* for_block = new_node(Block, NULL);
                            add_child($$, for_block);
                            add_child(for_block, $4);
                        }
                    |   RETURN OptExpr                                          
                        {
                            $$ = new_node(Return, NULL);
                            add_child($$, $2);

                            LOCATE($$, @1.first_line, @1.first_column);
                        }
                    |   FuncInvocation                                          
                        {
                            $$ = $1;
                        }
                    |   ParseArgs                                               
                        {
                            $$ = $1;
                        }
                    |   PRINT LPAR Expr RPAR                                    
                        {
                            $$ = new_node(Print, NULL);

                            //add_child($$, new_node(For, NULL));
                            add_child($$, $3);
                        }
                    |   PRINT LPAR STRLIT RPAR                                  
                        {
                            $$ = new_node(Print, NULL);
                            add_child($$, new_node(StrLit, $3));
                        }
                    |   error                                                   
                        {
                            $$ = new_node(AUX, NULL);
                        }
                    ;

StarStatementSc     :   StarStatementSc Statement SEMICOLON                     
                        {
                            $$ = $1;
                            add_child($$, $2);
                        }
                    |                                                           
                        {
                            $$ = new_node(AUX, NULL);
                        }
                    ;

OptElse             :   ELSE LBRACE StarStatementSc RBRACE                      
                        {
                            $$ = $3;
                        }
                    |                                                           
                        {
                            $$ = new_node(AUX, NULL);
                        }
                    ;

OptExpr             :   Expr                                                
                        {
                            $$ = $1;
                        }
                    |                                                       
                        {
                            $$ = new_node(AUX, NULL);
                        }
                    ;

ParseArgs           :   IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ Expr RSQ RPAR 
                        {
                            $$ = new_node(ParseArgs, NULL);
                            add_child($$, new_node(Identifier, $1));
                            add_child($$, $9);
                            
                            LOCATE($$, @5.first_line, @5.first_column);
                        }
                    |   IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR error RPAR 
                        {
                            $$ = new_node(AUX, NULL);
                        }
                    ;

FuncInvocation      :   IDENTIFIER LPAR PosExpr RPAR                            
                        {
                            $$ = new_node(Call, NULL);
                            add_child($$, new_node(Identifier, $1));
                            struct node *params = new_node(AUX, NULL);
                            add_child($$, params);
                            add_child($$, $3);

                            LOCATE(get_child($$, 0), @1.first_line, @1.first_column);
                        }
                    |   IDENTIFIER LPAR error RPAR                               
                        {
                            $$ = new_node(AUX, NULL);
                        }    
                    ;

PosExpr             :   Expr                                                    
                        {
                            $$ = new_node(AUX, NULL);
                            add_child($$, $1);
                        }
                    |   Expr COMMA PosExpr                                      
                        {
                            $$ = new_node(AUX, NULL);
                            add_child($$, $1);
                            add_child($$, $3);
                        }
                    |                                                           
                        {
                            $$ = new_node(AUX, NULL);
                        }
                    ;

Expr                :   Expr OR Expr                                            
                        {
                            $$ = new_node(Or, NULL);
                            add_child($$, $1);
                            add_child($$, $3);
                            LOCATE($$, @2.first_line, @2.first_column);
                        }
                    |   Expr AND Expr                                           
                        {
                            $$ = new_node(And, NULL);
                            add_child($$, $1);
                            add_child($$, $3);
                            LOCATE($$, @2.first_line, @2.first_column);
                        }

                    |   Expr LT Expr                                            
                        {
                            $$ = new_node(Lt, NULL);
                            add_child($$, $1);
                            add_child($$, $3);
                            LOCATE($$, @2.first_line, @2.first_column);
                        }
                    |   Expr GT Expr                                            
                        {
                            $$ = new_node(Gt, NULL);
                            add_child($$, $1);
                            add_child($$, $3);
                            LOCATE($$, @2.first_line, @2.first_column);
                        }
                    |   Expr EQ Expr                                            
                        {
                            $$ = new_node(Eq, NULL);
                            add_child($$, $1);
                            add_child($$, $3);
                            LOCATE($$, @2.first_line, @2.first_column);
                        }
                    |   Expr NE Expr                                            
                        {
                            $$ = new_node(Ne, NULL);
                            add_child($$, $1);
                            add_child($$, $3);
                            LOCATE($$, @2.first_line, @2.first_column);
                        }
                    |   Expr LE Expr                                            
                        {
                            $$ = new_node(Le, NULL);
                            add_child($$, $1);
                            add_child($$, $3);
                            LOCATE($$, @2.first_line, @2.first_column);
                        }
                    |   Expr GE Expr                                            
                        {
                            $$ = new_node(Ge, NULL);
                            add_child($$, $1);
                            add_child($$, $3);
                            LOCATE($$, @2.first_line, @2.first_column);
                        }
                    
                    |   Expr PLUS Expr                                          
                        {
                            $$ = new_node(Add, NULL);
                            add_child($$, $1);
                            add_child($$, $3);
                            LOCATE($$, @2.first_line, @2.first_column);
                        }
                    |   Expr MINUS Expr                                         
                        {
                            $$ = new_node(Sub, NULL);
                            add_child($$, $1);
                            add_child($$, $3);
                            LOCATE($$, @2.first_line, @2.first_column);
                        }
                    |   Expr STAR Expr                                          
                        {
                            $$ = new_node(Mul, NULL);
                            add_child($$, $1);
                            add_child($$, $3);
                            LOCATE($$, @2.first_line, @2.first_column);
                        }       
                    |   Expr DIV Expr                                           
                        {
                            $$ = new_node(Div, NULL);
                            add_child($$, $1);
                            add_child($$, $3);
                            LOCATE($$, @2.first_line, @2.first_column);
                        }
                    |   Expr MOD Expr                                           
                        {
                            $$ = new_node(Mod, NULL);
                            add_child($$, $1);
                            add_child($$, $3);
                            LOCATE($$, @2.first_line, @2.first_column);
                        }

                    |   MINUS Expr              %prec NOT                       
                        {
                            $$ = new_node(Minus, NULL);
                            add_child($$, $2);
                            LOCATE($$, @1.first_line, @1.first_column);
                        }
                    |   NOT Expr                %prec NOT                       
                        {
                            $$ = new_node(Not, NULL);
                            add_child($$, $2);
                            LOCATE($$, @1.first_line, @1.first_column);
                        }
                    |   PLUS Expr               %prec NOT                       
                        {
                            $$ = new_node(Plus, NULL);
                            add_child($$, $2);
                            LOCATE($$, @1.first_line, @1.first_column);
                        }
                    |   NATURAL                                                 
                        {
                            $$ = new_node(Natural, $1);
                            LOCATE($$, @1.first_line, @1.first_column);
                        }
                    |   DECIMAL                                                 
                        {
                            $$ = new_node(Decimal, $1);
                            LOCATE($$, @1.first_line, @1.first_column);
                        }
                    |   IDENTIFIER                                              
                        {
                            $$ = new_node(Identifier, $1);

                            LOCATE($$, @1.first_line, @1.first_column);
                        }
                    |   FuncInvocation                                          
                        {
                            $$ = $1;
                            LOCATE($$, @1.first_line, @1.first_column);
                        }
                    |   LPAR Expr RPAR                                          
                        {
                            $$ = $2;
                            
                        }
                    |   LPAR error RPAR                                         
                        {
                            $$ = new_node(AUX, NULL);
                        }
                    ;




%%

// all needed functions are collected in the .l and ast.* files