all: hello

hello:   test.l test.y
	flex test.l
	bison -d test.y
	gcc lex.yy.c test.tab.c main.c -o asShell

clean:
	rm -f *o asShell test.tab.h test.tab.c lex.yy.c

