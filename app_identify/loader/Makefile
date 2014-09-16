all:
	make prepare 

clean:
	rm -f lex.sig.c sig.tab.c sig.tab.h 

prepare: sig.tab.c lex.sig.c

sig.tab.c: sig.y 
	bison -d -o sig.tab.c sig.y

lex.sig.c: sig.l 
	flex -o lex.sig.c sig.l

