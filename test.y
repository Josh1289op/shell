%{
#define YYDEBUG 1
#include "global.h"
#define YYSTYPE char *
extern int line_num;
extern int isCommand, isCommandValue;

  void yyerror(const char *str)
{
        fprintf(stderr,"error: %s\n",  str);
}

int yywrap()
{
        return 1;
}


%}

%token COMMAND VALUE OPTION EOL
%%

start: command EOL  { return 0; }

command: COMMAND  axis {printf("Command %s\n", $1);}
      | COMMAND { printf("Command %s\n", $1); isBuiltin = true; isCommand = 1; cmd = $1;}
      | COMMAND VALUE { printf("COMMAND VALUE %s %s \n", $1, $2); isBuiltin = true; isCommandValue = 1; cmd = $1; value = $2;};

axis: inter | axis inter ;

inter: VALUE  {printf("Inter value %s\n", $1);}
       | OPTION {printf("Inter option %s\n", $1);}
%%
