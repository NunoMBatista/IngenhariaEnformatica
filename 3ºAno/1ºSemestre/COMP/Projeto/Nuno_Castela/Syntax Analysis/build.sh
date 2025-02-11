#!/bin/sh

rm -f gocompiler lex.yy.c y.tab.c y.tab.h y.output y.gv y.png gocompiler.zip
if [ "$1" = "clean" ]; then
    exit 0
fi

if [ "$1" = "zip" ]; then
    zip gocompiler.zip gocompiler.l gocompiler.y ast.h ast.c
    exit 0
fi

yacc -d -v -t -g --report=all gocompiler.y
lex gocompiler.l
#dot y.gv -Tpng -o y.png               # LALR automaton in y.png
gcc -g -o gocompiler lex.yy.c y.tab.c ast.c -Wall -Wextra -Wno-unused-function

