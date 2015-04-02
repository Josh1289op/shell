all: hello

hello:   test.l test.y
	rm -f *o asShell test.tab.h test.tab.c lex.yy.c
	flex test.l
	bison -d test.y
	gcc lex.yy.c test.tab.c main.c -o asShell

clean:
	rm -f *o asShell test.tab.h test.tab.c lex.yy.c

push:
	rm -f *o asShell test.tab.h test.tab.c lex.yy.c
	git add .
	git commit -a -m "Pushed from Make File"
	git pull origin master
	git push origin master

start: test.l test.y
	rm -f *o asShell test.tab.h test.tab.c lex.yy.c
	flex test.l
	bison -d test.y
	gcc lex.yy.c test.tab.c main.c -o asShell
	./asShell