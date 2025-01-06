%{
    /*
        Authors:
            Nuno Batista: uc2022216127
            Miguel Castela: uc2022212972
    */
    #include <ctype.h>
    #include <stdio.h>
    #include <string.h>
    #include "y.tab.h"
    #include "ast.h"
    #include "semantics.h"
    #include "codegen.h"

   // #define DEBUG

    #ifdef DEBUG
        extern int yydebug;
    #endif

    // This macro is executed before each token is returned
    #define YY_USER_ACTION \
        syn_line = yylloc.first_line = lex_line; \
        syn_col = yylloc.first_column = lex_col; \
        lex_col += yyleng;


    #define PASS_TOKEN() yylval.lexeme = strdup(yytext);

    int yyparse(void);
    void yyerror(char*);
    int errors = 0;

    int lex_only = 0;

    // Lexical analyser current line and column
    int lex_line = 1, lex_col = 1;

    // Syntax analyser current line and column
    int syn_col = -1, syn_line = -1;

    // Flag to add semicolon after certain tokens
    int add_semicolon = 0;

    // Multi-line comment start line and column
    int ml_comment_start_line = 0;
    int ml_comment_start_col = 0;

    // String literal start line and column
    int str_start_line = 0;
    int str_start_col = 0;

    // Buffer to store string literals
    char str_buffer[4096];
    int str_buffer_index = 0;

    // Flag to indicate if a string literal has an error
    int str_error = 0;

    // Flag to indicate if the lexer should return EOF
    int return_eof = 0;

    extern struct node *program;

    // Function to add a string to the current buffer
    void add_str_buffer(char *add, int length) {
        strncpy(&str_buffer[str_buffer_index], add, length);
        str_buffer_index += length;
    }

    // Function to reset the string buffer
    void reset_str_buffer() {
        str_buffer_index = 0;
        str_buffer[0] = '\0';
    }

    extern struct symbol_list *symbol_table;
    extern struct scopes_queue *symbol_scopes;

%}

SEMICOLON                           ";"
COMMA                               ","
BLANKID                             "_"
ASSIGN                              "="
STAR                                "*"
DIV                                 "/"
MINUS                               "-"
PLUS                                "+"
EQ                                  "=="
GE                                  ">="
GT                                  ">"
LBRACE                              "{"
LE                                  "<="
LPAR                                "("
LSQ                                 "["
LT                                  "<"
MOD                                 "%"
NE                                  "!="
NOT                                 "!"
AND                                 "&&"
OR                                  "||"
RBRACE                              "}"
RPAR                                ")"
RSQ                                 "]"
PACKAGE                             "package"
RETURN                              "return"
ELSE                                "else"
FOR                                 "for"
IF                                  "if"
VAR                                 "var"
INT                                 "int"
FLOAT32                             "float32"
BOOL                                "bool"
STRING                              "string"
PRINT                               "fmt.Println"
PARSEINT                            "strconv.Atoi"
FUNC                                "func"
CMDARGS                             "os.Args"
RESERVED                            "++"|"--"|"break"|"case"|"chan"|"const"|"continue"|"default"|"defer"|"fallthrough"|"go"|"goto"|"import"|"interface"|"map"|"range"|"select"|"struct"|"switch"|"type"

IGNORE                              " "|"\t"|"\f"
NEWLINE                             "\n"|"\r"|"\r\n"

LETTER                              [a-z]|[A-Z]|"_"
DECLIT                              [0-9]

OCTALPREFIX                         "0"
OCTAL                               {OCTALPREFIX}[0-7]+
INVALID_OCTAL                       {OCTALPREFIX}[0-9]+

HEXPREFIX                            "0x"|"0X"
HEX                                  {HEXPREFIX}[0-9a-fA-F]+

IDENTIFIER                          {LETTER}({DECLIT}|{LETTER})*
NATURAL                             {DECLIT}+

EXPONENT                            [eE][+-]?{DECLIT}+
DECIMAL                             ({DECLIT}+"."{DECLIT}*{EXPONENT}?)|("."{DECLIT}+{EXPONENT}?)|({DECLIT}+{EXPONENT})

STRLIT                              ([^\n\r\\"]|\\n|\\f|\\r|\\\\|\\\"|\\t)*

QUOTE                               \"

%X SINGLE_LINE_COMMENT
%X MULTI_LINE_COMMENT
%X STRING_LITERAL

%%

"//"                                { BEGIN(SINGLE_LINE_COMMENT); }
<SINGLE_LINE_COMMENT><<EOF>>        { BEGIN(INITIAL); printf("Line %d, column %d: unterminated comment\n", lex_line, lex_col - yyleng); add_semicolon = 0; }
<SINGLE_LINE_COMMENT>{NEWLINE}      { BEGIN(INITIAL); lex_line++; /*syn_line++; syn_col = 1;*/  lex_col = 1; if(add_semicolon) { add_semicolon = 0; if(lex_only) printf("SEMICOLON\n"); else return SEMICOLON; } }
<SINGLE_LINE_COMMENT>.              { ; }

"/*"                                { BEGIN(MULTI_LINE_COMMENT); ml_comment_start_line = lex_line; ml_comment_start_col = lex_col - yyleng; }
<MULTI_LINE_COMMENT><<EOF>>         { printf("Line %d, column %d: unterminated comment\n", ml_comment_start_line, ml_comment_start_col); BEGIN(INITIAL); add_semicolon = 0; }
<MULTI_LINE_COMMENT>"*/"            { BEGIN(INITIAL); }
<MULTI_LINE_COMMENT>{NEWLINE}       { lex_line++; /*syn_line++; syn_col = 1;*/ lex_col = 1; }
<MULTI_LINE_COMMENT>.               { ; }



{QUOTE}                             { BEGIN(STRING_LITERAL); add_semicolon = 1; add_str_buffer(yytext, yyleng); str_error = 0; str_start_line = lex_line; str_start_col = lex_col - yyleng; }
<STRING_LITERAL><<EOF>>             { printf("Line %d, column %d: unterminated string literal\n", str_start_line, str_start_col);  reset_str_buffer(); add_semicolon = 0; BEGIN(INITIAL); }
<STRING_LITERAL>{STRLIT}            { add_str_buffer(yytext, yyleng); }
<STRING_LITERAL>\\(.?)              { printf("Line %d, column %d: invalid escape sequence (%s)\n", str_start_line, lex_col - yyleng, yytext); str_error = 1; str_buffer_index += yyleng; /*BEGIN(INITIAL); reset_str_buffer();*/ }
<STRING_LITERAL>{NEWLINE}           { printf("Line %d, column %d: unterminated string literal\n", str_start_line, str_start_col); BEGIN(INITIAL); lex_line++; syn_line++; syn_col = 1; lex_col = 1; reset_str_buffer(); add_semicolon = 0; }
<STRING_LITERAL>{QUOTE}             {
                                        add_str_buffer(yytext, yyleng);
                                        str_buffer[str_buffer_index] = '\0';

                                        if(!str_error){
                                            if(lex_only){
                                                printf("STRLIT(%s)\n", str_buffer);
                                            }
                                            else{
                                                // Store the string literal in the lexeme field
                                                yylval.lexeme = strdup(str_buffer);

                                                // Reset and change state before returning token
                                                reset_str_buffer();
                                                BEGIN(INITIAL);
                                                return STRLIT;
                                            }
                                            add_semicolon = 1;
                                        }
                                        else{
                                            add_semicolon = 0;
                                        }

                                        reset_str_buffer();
                                        BEGIN(INITIAL);
                                    }



{SEMICOLON}                         { if(lex_only) printf("SEMICOLON\n");               add_semicolon = 0;      if(!lex_only) { return SEMICOLON; } }
{COMMA}                             { if(lex_only) printf("COMMA\n");                   add_semicolon = 0;      if(!lex_only) { return COMMA; } }
{ASSIGN}                            { if(lex_only) printf("ASSIGN\n");                  add_semicolon = 0;      if(!lex_only) { return ASSIGN; } }
{STAR}                              { if(lex_only) printf("STAR\n");                    add_semicolon = 0;      if(!lex_only) { return STAR; } }
{DIV}                               { if(lex_only) printf("DIV\n");                     add_semicolon = 0;      if(!lex_only) { return DIV; } }
{MINUS}                             { if(lex_only) printf("MINUS\n");                   add_semicolon = 0;      if(!lex_only) { return MINUS; } }
{PLUS}                              { if(lex_only) printf("PLUS\n");                    add_semicolon = 0;      if(!lex_only) { return PLUS; } }
{EQ}                                { if(lex_only) printf("EQ\n");                      add_semicolon = 0;      if(!lex_only) { return EQ; } }
{GE}                                { if(lex_only) printf("GE\n");                      add_semicolon = 0;      if(!lex_only) { return GE; } }
{GT}                                { if(lex_only) printf("GT\n");                      add_semicolon = 0;      if(!lex_only) { return GT; } }
{LBRACE}                            { if(lex_only) printf("LBRACE\n");                  add_semicolon = 0;      if(!lex_only) { return LBRACE; } }
{LE}                                { if(lex_only) printf("LE\n");                      add_semicolon = 0;      if(!lex_only) { return LE; } }
{LPAR}                              { if(lex_only) printf("LPAR\n");                    add_semicolon = 0;      if(!lex_only) { return LPAR; } }
{LSQ}                               { if(lex_only) printf("LSQ\n");                     add_semicolon = 0;      if(!lex_only) { return LSQ; } }
{LT}                                { if(lex_only) printf("LT\n");                      add_semicolon = 0;      if(!lex_only) { return LT; } }
{MOD}                               { if(lex_only) printf("MOD\n");                     add_semicolon = 0;      if(!lex_only) { return MOD; } }
{NE}                                { if(lex_only) printf("NE\n");                      add_semicolon = 0;      if(!lex_only) { return NE; } }
{NOT}                               { if(lex_only) printf("NOT\n");                     add_semicolon = 0;      if(!lex_only) { return NOT; } }
{AND}                               { if(lex_only) printf("AND\n");                     add_semicolon = 0;      if(!lex_only) { return AND; } }
{OR}                                { if(lex_only) printf("OR\n");                      add_semicolon = 0;      if(!lex_only) { return OR; } }
{PACKAGE}                           { if(lex_only) printf("PACKAGE\n");                 add_semicolon = 0;      if(!lex_only) { return PACKAGE; } }
{ELSE}                              { if(lex_only) printf("ELSE\n");                    add_semicolon = 0;      if(!lex_only) { return ELSE; } }
{FOR}                               { if(lex_only) printf("FOR\n");                     add_semicolon = 0;      if(!lex_only) { return FOR; } }
{IF}                                { if(lex_only) printf("IF\n");                      add_semicolon = 0;      if(!lex_only) { return IF; } }
{VAR}                               { if(lex_only) printf("VAR\n");                     add_semicolon = 0;      if(!lex_only) { return VAR; } }
{INT}                               { if(lex_only) printf("INT\n");                     add_semicolon = 0;      if(!lex_only) { return INT; } }
{FLOAT32}                           { if(lex_only) printf("FLOAT32\n");                 add_semicolon = 0;      if(!lex_only) { return FLOAT32; } }
{BOOL}                              { if(lex_only) printf("BOOL\n");                    add_semicolon = 0;      if(!lex_only) { return BOOL; } }
{STRING}                            { if(lex_only) printf("STRING\n");                  add_semicolon = 0;      if(!lex_only) { return STRING; } }
{PRINT}                             { if(lex_only) printf("PRINT\n");                   add_semicolon = 0;      if(!lex_only) { return PRINT; } }
{PARSEINT}                          { if(lex_only) printf("PARSEINT\n");                add_semicolon = 0;      if(!lex_only) { return PARSEINT; } }
{FUNC}                              { if(lex_only) printf("FUNC\n");                    add_semicolon = 0;      if(!lex_only) { return FUNC; } }
{CMDARGS}                           { if(lex_only) printf("CMDARGS\n");                 add_semicolon = 0;      if(!lex_only) { return CMDARGS; } }
{IGNORE}                            { ; }

{BLANKID}                           { if(lex_only) printf("BLANKID\n");                 add_semicolon = 0;      if(!lex_only) { return BLANKID; } }
{RESERVED}                          { if(lex_only) printf("RESERVED(%s)\n", yytext);    add_semicolon = 0;      if(!lex_only) { PASS_TOKEN(); return RESERVED; } }

{RBRACE}                            { if(lex_only) printf("RBRACE\n");                  add_semicolon = 1;      if(!lex_only) { return RBRACE; } }
{RPAR}                              { if(lex_only) printf("RPAR\n");                    add_semicolon = 1;      if(!lex_only) { return RPAR; } }
{RSQ}                               { if(lex_only) printf("RSQ\n");                     add_semicolon = 1;      if(!lex_only) { return RSQ; } }
{RETURN}                            { if(lex_only) printf("RETURN\n");                  add_semicolon = 1;      if(!lex_only) { return RETURN; } }
{IDENTIFIER}                        { if(lex_only) printf("IDENTIFIER(%s)\n", yytext);  add_semicolon = 1;      if(!lex_only) { PASS_TOKEN(); return IDENTIFIER; } }

{OCTAL}                             { if(lex_only) printf("NATURAL(%s)\n", yytext);     add_semicolon = 1;          if(!lex_only) { PASS_TOKEN(); return NATURAL; } }
{INVALID_OCTAL}                     { printf("Line %d, column %d: invalid octal constant (%s)\n", lex_line, lex_col - yyleng, yytext); add_semicolon = 0; }
{NATURAL}                           { if(lex_only) printf("NATURAL(%s)\n", yytext);     add_semicolon = 1;          if(!lex_only) { PASS_TOKEN(); return NATURAL; } }
{HEX}                               { if(lex_only) printf("NATURAL(%s)\n", yytext);     add_semicolon = 1;          if(!lex_only) { PASS_TOKEN(); return NATURAL; } }

{DECIMAL}                           { if(lex_only) printf("DECIMAL(%s)\n", yytext);     add_semicolon = 1;          if(!lex_only) { PASS_TOKEN(); return DECIMAL; } }

{NEWLINE}                           {
                                        lex_line++; /*syn_line++; syn_col = 1;*/  lex_col = 1;

                                        if(add_semicolon) {
                                            add_semicolon = 0;
                                            if(lex_only == 1) {
                                                printf("SEMICOLON\n");
                                            }
                                            if(lex_only == 0) {
                                                return SEMICOLON;
                                            }
                                        }
                                    }

<<EOF>>                             {
                                        YY_USER_ACTION;
                                        lex_col--;

                                        if(return_eof) {
                                            return 0;
                                        }

                                        if(add_semicolon) {
                                            if(lex_only == 1){
                                                printf("SEMICOLON\n");
                                            }
                                            if(lex_only == 0) {
                                                return_eof = 1;
                                                return SEMICOLON;
                                            }

                                            add_semicolon = 0;
                                        };
                                        return_eof = 1;
                                    }

.                                   { printf("Line %d, column %d: illegal character (%s)\n", lex_line, lex_col - yyleng, yytext); }

%%

void yyerror(char *error) {
    errors++;
    // If the error is in a string literal, print the error with the string literal
    if((yylval.lexeme != NULL) && (yylval.lexeme[0] == '"')){
        printf("Line %d, column %d: %s: %s\n", str_start_line, str_start_col, error, yylval.lexeme);
        free(yylval.lexeme);
        yylval.lexeme = NULL;
        reset_str_buffer();
    }
    else {
        printf("Line %d, column %d: %s: %s\n", syn_line, syn_col, error, yytext);
    }

}

extern int yylex();
int main(int argc, char **argv) {
    #ifdef DEBUG
        yydebug = 1;
    #endif

    if(argc > 1) {
        if(strcmp(argv[1], "-l") == 0) {
            lex_only = 1;
            yylex();    /* run the lexical analysis automaton */
        }
        if(strcmp(argv[1], "-t") == 0) {
            yyparse(); /* run the syntax analysis automaton and print the AST */
                remove_aux(program);
            if(errors == 0) {
                dfs(program, 0);
            }
        }
        if(strcmp(argv[1], "-s") == 0) {
            yyparse(); /* run the syntax analysis automaton */
            remove_aux(program);

            if(errors == 0) {
                // Check semantics
                errors += check_program(program);
                /* if(errors > 0){
                    printf("\n");
                } */

                show_symbol_table();
                show_symbol_scopes();

                // Print annotated AST
                dfs(program, 0);

            }
        }
    }
    else{
        yyparse();
        remove_aux(program);
        if(errors == 0) {
            errors += check_program(program);
        }
    }

    if((argc == 1) && (errors == 0)){
        codegen_program(program);
    }

    // Free AST
    if(program != NULL){
        free_ast(program);
    }

    // Free symbol table
    if(symbol_table != NULL){
        free_symbol_table(symbol_table);
    }

    // Free scopes queue
    if(symbol_scopes != NULL){
        free_symbol_scopes();
    }

    // Free lexer
    yylex_destroy();

    return 0;
}
int yywrap() {  /* called on EOF, return 1 to terminate */
    return 1;
}
