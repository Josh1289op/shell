%{
#include "global.h"

void yyerror(const char *str)
{
        fprintf(stderr,"error: %s\n",str);
}
 
int yywrap()
{
        return 1;
} 
  

%}

%token STOP START CD LS NUMBER WORD EOL
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
        | eol_com
        ;

change_dir:
        CD EOL      { cd = 1; word = "home"; return;}
		| CD WORD	{ cd = 1; word = yylval.a; return;}
        ;

list_files:
        LS

start_com:
        START
            {
                    printf("\t START");
                    return;
            }
        ;

stop_com:
        STOP
            {
                    printf("\t STOP\n");
                    exit(0);
            }
        ;

eol_com:
        EOL     {return;}
%%
