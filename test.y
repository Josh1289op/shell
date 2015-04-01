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

%token STOP START NUMBER WORD
%%
commands: /* empty */
        | commands command
        ;

command:
        start_com
        |
        stop_com
        ;

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
