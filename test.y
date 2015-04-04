%{
<<<<<<< HEAD
#define YYDEBUG 0
=======
//#define YYDEBUG 1
>>>>>>> 525d0946e1e0f1c57d2fa36c996ee2bd8721e127
#include "global.h"
#define YYSTYPE char *
extern int line_num;
extern int isCommand, isCommandValue, valuecount;

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

command: COMMAND  axis {printf("Command %s\n", $1); isBuiltin = true; cmd = $1;}
      | COMMAND { printf("Command %s\n", $1); isBuiltin = true; cmd = $1;};

axis: inter | axis inter ;

inter: VALUE  {printf("Inter value %s\n", $1);
               isBuiltin = true; isCommandValue = 1; value[valuecount++] = $1;
              }
       | OPTION {printf("Inter option %s\n", $1);}
%%
