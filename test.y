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
	   | VALUE command EOL { insertCmd.name = $1; return 1; }
	   | OPTION command EOL { insertCmd.name = $1; return 1; };

command:  { insertCmd.name = "empty"; }
	  |	COMMAND  axis { printf("Command %s\n", $1); 
	  					insertCmd.name = $1; 
	  					insertCmd.args[0] = insertCmd.name;
	  					cmdTab[numTabCmds++] = insertCmd;
	  				  }
      | COMMAND { printf("Command %s\n", $1); 
      			  insertCmd.name = $1; 
      			  insertCmd.args[0] = insertCmd.name;
      			  cmdTab[numTabCmds++] = insertCmd;
      			  }
      | VALUE axis | OPTION axis | VALUE | OPTION;

axis: inter | axis inter ;

inter: VALUE  { isCommandValue = true;
                insertCmd.args[++insertCmd.numArgs] = $1;
                printf("Inter value %s\n", insertCmd.args[insertCmd.numArgs]);
              }
       | OPTION { insertCmd.args[++insertCmd.numArgs] = $1;
       			  printf("Inter option %s\n", insertCmd.args[insertCmd.numArgs]);
       			};
%%
