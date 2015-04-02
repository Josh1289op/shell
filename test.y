%{
#include "global.h"

void yyerror(const char *str)
{
        fprintf(stderr,"error: %s",str);
}
 
int yywrap()
{
        return 1;
} 
  

%}

%token STOP START CD NUMBER WORD
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
		|
        start_com
        |
        stop_com
        ;
change_dir:
		CD WORD
		{
			cd = 1;
			changeDirectory = yylval.a;
			printf("\t change_dir init cd = 1");
			return;
		}
start_com:
         START
        {
                printf("\t START \n");
        }
        ;
stop_com:
        STOP
        {
                printf("\t STOP \n");
        }
        ;
%%
