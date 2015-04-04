%{
#define YYDEBUG 0
#include "global.h"
#define YYSTYPE char *
extern int line_num;
extern int isCommand, isCommandValue, valuecount;

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
	  |	COMMAND  axis {printf("Command %s\n", $1); curCmd.name = $1;}
      | COMMAND { printf("Command %s\n", $1); curCmd.name = $1;};

axis: inter | axis inter ;

inter: VALUE  { printf("Inter value %s\n", $1);
                isCommandValue = true; 
                value[valuecount++] = $1;
              }
       | OPTION { printf("Inter option %s\n", $1); }
%%
