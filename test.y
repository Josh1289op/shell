%{
#ifndef YYSTYPE
#define YYSTYPE char*
#endif
#define YYDEBUG 0
#include "global.h"

extern int isCommandValue;

  void yyerror(const char *str)
{
        //fprintf(stderr,"error: %s\n",  str);
}

int yywrap()
{
        return 1;
}

%}

%token COMMAND VALUE OPTION EOL 

%%

start: command EOL  { return 0; }

command:  { curCmd.name = "empty"; }
	  |	COMMAND  axis {printf("Command %s\n", $1); curCmd.name = $1; curCmd.args[0] = curCmd.name;}
      | COMMAND { printf("Command %s\n", $1); curCmd.name = $1; curCmd.args[0] = curCmd.name;};

axis: inter | axis inter ;

inter: VALUE  { isCommandValue = true;
                curCmd.args[++curCmd.numArgs] = $1;
                printf("Inter value %s\n", curCmd.args[curCmd.numArgs]);
              }
       | OPTION { curCmd.args[++curCmd.numArgs] = $1;
       			  printf("Inter option %s\n", curCmd.args[curCmd.numArgs]);
       			}
%%
