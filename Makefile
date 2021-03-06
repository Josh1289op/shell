all: hello

hello:   test.l test.y
	rm -f *o asShell test.tab.h test.tab.c lex.yy.c
	flex test.l
	bison -d test.y
	gcc lex.yy.c test.tab.c main.c -o asShell -g

clean:
	rm -f *o asShell test.tab.h test.tab.c lex.yy.c test.output

push:
	rm -f *o asShell test.tab.h test.tab.c lex.yy.c
	git add .
	git commit -a -m "Pushed from Make File"
	git pull origin master
	git push origin master
	
cpush:
	rm -f *o asShell test.tab.h test.tab.c lex.yy.c
	git add .
	git commit -a -m "$(c)"
	git pull origin master
	git push origin master

start: test.l test.y
	rm -f *o asShell test.tab.h test.tab.c lex.yy.c
	flex test.l
	bison -d test.y
	gcc lex.yy.c test.tab.c main.c -o asShell
	./asShell
	
run: test.l test.y
	@-rm -f *o asShell test.tab.h test.tab.c lex.yy.c
	@-flex test.l
	@-bison -d test.y
	@-gcc lex.yy.c test.tab.c main.c -o asShell -g
	@./asShell
	
debug: test.l test.y
	rm -f *o asShell test.tab.h test.tab.c lex.yy.c
	flex test.l
	bison -dv test.y
	gcc lex.yy.c test.tab.c main.c -o asShell -s -Q
	
	
runFNF: test.l test.y
	@-rm -f *o asShell test.tab.h test.tab.c lex.yy.c
	@-flex test.l
	@-bison -d test.y
	@-gcc lex.yy.c test.tab.c main.c -o asShell -g
	@-./asShell hey.txt
	
runFF: test.l test.y
	@-rm -f *o asShell test.tab.h test.tab.c lex.yy.c
	@-flex test.l
	@-bison -d test.y
	@-gcc lex.yy.c test.tab.c main.c -o asShell -g
	@-./asShell testCases	
