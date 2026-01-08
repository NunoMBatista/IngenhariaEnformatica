%{
    /*
        Authors:
            Nuno Batista: uc2022216127
            Miguel Castela: uc2022212972
    */

    #include <ctype.h>
    #include <stdio.h>
    #include <string.h>

    #define YY_USER_ACTION \
        syn_col = curcol; \
        curcol += yyleng; \
        
        
    int verbose = 0;

    int curline = 1; 
    int curcol = 1;
    int syn_col = 1; 
    int add_semicolon = 0; 

    int ml_comment_start_line = 0;
    int ml_comment_start_col = 0;

    int str_start_line = 0;
    int str_start_col = 0;

    char str_buffer[4096];
    int str_buffer_index = 0;

    int str_error = 0;

    void add_str_buffer(char *add, int length) {
        strncpy(&str_buffer[str_buffer_index], add, length);
        str_buffer_index += length;
    }

    void reset_str_buffer() {
        str_buffer_index = 0;
        str_buffer[0] = '\0';
    }

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
%X STRING

%%


"//"                                { BEGIN(SINGLE_LINE_COMMENT); }
<SINGLE_LINE_COMMENT><<EOF>>        { BEGIN(INITIAL); printf("Line %d, column %d: unterminated comment\n", curline, curcol - yyleng); add_semicolon = 0; }
<SINGLE_LINE_COMMENT>{NEWLINE}      { BEGIN(INITIAL); if(add_semicolon && verbose) printf("SEMICOLON\n"); add_semicolon = 0; curline++; curcol = 1; }
<SINGLE_LINE_COMMENT>.              { ; }

"/*"                                { BEGIN(MULTI_LINE_COMMENT); ml_comment_start_line = curline; ml_comment_start_col = curcol - yyleng; }
<MULTI_LINE_COMMENT><<EOF>>         { printf("Line %d, column %d: unterminated comment\n", ml_comment_start_line, ml_comment_start_col); BEGIN(INITIAL); add_semicolon = 0; }
<MULTI_LINE_COMMENT>"*/"            { BEGIN(INITIAL); }
<MULTI_LINE_COMMENT>{NEWLINE}       { curline++; curcol = 1; }
<MULTI_LINE_COMMENT>.               { ; }



{QUOTE}                             { BEGIN(STRING); add_semicolon = 1; add_str_buffer(yytext, yyleng); str_error = 0; str_start_line = curline; str_start_col = curcol - yyleng; }
<STRING><<EOF>>                     { printf("Line %d, column %d: unterminated string literal\n", str_start_line, str_start_col);  reset_str_buffer(); add_semicolon = 0; BEGIN(INITIAL); }
<STRING>{STRLIT}                    { add_str_buffer(yytext, yyleng); }
<STRING>\\(.?)                      { printf("Line %d, column %d: invalid escape sequence (%s)\n", str_start_line, curcol - yyleng, yytext); str_error = 1; str_buffer_index += yyleng; /*BEGIN(INITIAL); reset_str_buffer();*/ }
<STRING>{NEWLINE}                   { printf("Line %d, column %d: unterminated string literal\n", str_start_line, str_start_col); BEGIN(INITIAL); curline++; curcol = 1; reset_str_buffer(); add_semicolon = 0; }
<STRING>{QUOTE}                     { 
                                        add_str_buffer(yytext, yyleng); 
                                        str_buffer[str_buffer_index] = '\0';
                                        
                                        if(!str_error){
                                            if(verbose) printf("STRLIT(%s)\n", str_buffer); 
                                            add_semicolon = 1; 
                                        }
                                        else{
                                            add_semicolon = 0;
                                        }

                                        reset_str_buffer();
                                        BEGIN(INITIAL); 
                                    }



{SEMICOLON}                         { if(verbose) printf("SEMICOLON\n"); add_semicolon = 0; }
{COMMA}                             { if(verbose) printf("COMMA\n"); add_semicolon = 0; }
{ASSIGN}                            { if(verbose) printf("ASSIGN\n"); add_semicolon = 0; }
{STAR}                              { if(verbose) printf("STAR\n"); add_semicolon = 0; }
{DIV}                               { if(verbose) printf("DIV\n"); add_semicolon = 0; }
{MINUS}                             { if(verbose) printf("MINUS\n"); add_semicolon = 0; }
{PLUS}                              { if(verbose) printf("PLUS\n"); add_semicolon = 0; }
{EQ}                                { if(verbose) printf("EQ\n"); add_semicolon = 0; }
{GE}                                { if(verbose) printf("GE\n"); add_semicolon = 0; }
{GT}                                { if(verbose) printf("GT\n"); add_semicolon = 0; }
{LBRACE}                            { if(verbose) printf("LBRACE\n"); add_semicolon = 0; }
{LE}                                { if(verbose) printf("LE\n"); add_semicolon = 0; }
{LPAR}                              { if(verbose) printf("LPAR\n"); add_semicolon = 0; }
{LSQ}                               { if(verbose) printf("LSQ\n"); add_semicolon = 0; }
{LT}                                { if(verbose) printf("LT\n"); add_semicolon = 0; }
{MOD}                               { if(verbose) printf("MOD\n"); add_semicolon = 0; }
{NE}                                { if(verbose) printf("NE\n"); add_semicolon = 0; }
{NOT}                               { if(verbose) printf("NOT\n"); add_semicolon = 0; }
{AND}                               { if(verbose) printf("AND\n"); add_semicolon = 0; }
{OR}                                { if(verbose) printf("OR\n"); add_semicolon = 0; }
{PACKAGE}                           { if(verbose) printf("PACKAGE\n"); add_semicolon = 0; }
{ELSE}                              { if(verbose) printf("ELSE\n"); add_semicolon = 0; }
{FOR}                               { if(verbose) printf("FOR\n"); add_semicolon = 0; }
{IF}                                { if(verbose) printf("IF\n"); add_semicolon = 0; }
{VAR}                               { if(verbose) printf("VAR\n"); add_semicolon = 0; }
{INT}                               { if(verbose) printf("INT\n"); add_semicolon = 0; }
{FLOAT32}                           { if(verbose) printf("FLOAT32\n"); add_semicolon = 0; }
{BOOL}                              { if(verbose) printf("BOOL\n"); add_semicolon = 0; }
{STRING}                            { if(verbose) printf("STRING\n"); add_semicolon = 0; }
{PRINT}                             { if(verbose) printf("PRINT\n"); add_semicolon = 0; }
{PARSEINT}                          { if(verbose) printf("PARSEINT\n"); add_semicolon = 0; }
{FUNC}                              { if(verbose) printf("FUNC\n"); add_semicolon = 0; }
{CMDARGS}                           { if(verbose) printf("CMDARGS\n"); add_semicolon = 0; }
{IGNORE}                            { ; }

{BLANKID}                           { if(verbose) printf("BLANKID\n"); add_semicolon = 0; }
{RESERVED}                          { if(verbose) printf("RESERVED(%s)\n", yytext); add_semicolon = 0; }

{RBRACE}                            { if(verbose) printf("RBRACE\n"); add_semicolon = 1; }
{RPAR}                              { if(verbose) printf("RPAR\n"); add_semicolon = 1; }
{RSQ}                               { if(verbose) printf("RSQ\n"); add_semicolon = 1; }
{RETURN}                            { if(verbose) printf("RETURN\n"); add_semicolon = 1; }
{IDENTIFIER}                        { if(verbose) printf("IDENTIFIER(%s)\n", yytext); add_semicolon = 1; }


{OCTAL}                             { if(verbose) printf("NATURAL(%s)\n", yytext); add_semicolon = 1; }
{INVALID_OCTAL}                     { printf("Line %d, column %d: invalid octal constant (%s)\n", curline, curcol - yyleng, yytext); add_semicolon = 0; }
{NATURAL}                           { if(verbose) printf("NATURAL(%s)\n", yytext); add_semicolon = 1; }
{HEX}                               { if(verbose) printf("NATURAL(%s)\n", yytext); add_semicolon = 1; }

{DECIMAL}                           { if(verbose) printf("DECIMAL(%s)\n", yytext); add_semicolon = 1; }
{NEWLINE}                           { curline++; curcol = 1; if(add_semicolon && verbose) printf("SEMICOLON\n"); add_semicolon = 0; }

<<EOF>>                             { if(add_semicolon && verbose) printf("SEMICOLON\n"); add_semicolon = 0; return 0; }
.                                   { printf("Line %d, column %d: illegal character (%s)\n", curline, curcol - yyleng, yytext); }

%%

extern int yylex();
int main(int argc, char **argv) {

    if(argc > 1) {
        if(strcmp(argv[1], "-l") == 0) {
            verbose = 1;
        }
    }

    yylex();    /* run the lexical analysis automaton */
    return 0;
}
int yywrap() {  /* called on EOF, return 1 to terminate */
    return 1;
}
