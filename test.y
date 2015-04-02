%{
#include "global.h"
 extern int line_num;

 void yyerror(const char *str)
{
        fprintf(stderr,"error: %s\n",  str);
}

int yywrap()
{
        return 1;
} 
  

%}

%token START CD LS NUMBER WORD EOL STOP
%union {

  char *a;

  double d;

  int fn;
} 
%%
commands: /* empty */
        | commands command
        ;

command:
		change_dir
        | list_files
		| start_com
        | stop_com
        | unknown_com
        ;

change_dir:
        change_dir WORD  { isBuiltin = true; word = yylval.a; builtin = CDPath; }
		| CD         { isBuiltin = true; word = "home"; builtin = CDHome; }
        ;

list_files:
        LS

start_com:
        START {  isBuiltin = true; word = "start"; builtin = Start; }
        ;

stop_com:
        STOP { isBuiltin = true; word = "stop"; builtin = Stop; }
        ;



unknown_com:
        WORD { printf("command not recognized in .y: %s\n", yylval.a); }
%%
