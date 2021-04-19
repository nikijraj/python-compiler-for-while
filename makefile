Test.out : lex.yy.c y.tab.c y.tab.h 
	gcc lex.yy.c y.tab.c -g -lm -lfl -o Test.out

y.tab.c : python_compiler.y
	yacc -dv python_compiler.y

lex.yy.c : python_compiler.l
	lex python_compiler.l

clean :
	rm lex.yy.c y.tab.c y.tab.h Test.out y.output
