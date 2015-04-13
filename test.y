%{
#ifndef YYSTYPE
#define YYSTYPE char*
#endif
#define YYDEBUG 1
#include "global.h"

  void yyerror(const char *str)
{
        //fprintf(stderr,"error: %s\n",  str);
}

int yywrap()
{
        return 1;
}

%}

%token PIPE LEFT_OPEN RIGHT_OPEN ERROR_CMD COMMAND VALUE OPTION EOL 

%%

start: command EOL  { return 0; };

command:  { insertCmd.name = "empty"; insertCmd.args[0] = insertCmd.name; cmdTab[numTabCmds++] = insertCmd;}
      | command pipe command 
	    |	COMMAND  axis { //printf("Command axis %s\n", $1); 
	  					insertCmd.name = $1; 
	  					insertCmd.args[0] = insertCmd.name;
	  					if(swapping == true){
							cmdTab[tempNumTabCmds++] = insertCmd;
						}else{
							cmdTab[numTabCmds++] = insertCmd;
						}
	  				  }
      | COMMAND { //printf("Command %s\n", $1); 
      			  insertCmd.name = $1; 
      			  insertCmd.args[0] = insertCmd.name;
  	  		if(swapping == true){
				cmdTab[tempNumTabCmds++] = insertCmd;
			}else{
				cmdTab[numTabCmds++] = insertCmd;
			}
      			  }
      | VALUE axis  { insertCmd.name = $1; 
      					insertCmd.args[0] = insertCmd.name;
	  		if(swapping == true){
				cmdTab[tempNumTabCmds++] = insertCmd;
			}else{
				cmdTab[numTabCmds++] = insertCmd;
			} 
		}
      | VALUE   { //printf("value %s\n", $1);  
                		insertCmd.name = $1; 
      			insertCmd.args[0] = insertCmd.name;
	  		if(swapping == true){
				cmdTab[tempNumTabCmds++] = insertCmd;
			}else{
				cmdTab[numTabCmds++] = insertCmd;
			} 
 		}
      | OPTION axis { insertCmd.name = $1; return 1; }
      | OPTION      { insertCmd.name = $1; return 1; };

axis: inter | axis inter ;

inter: VALUE  { insertCmd.isCommandValue = true;
                insertCmd.args[++insertCmd.numArgs] = $1;
                //printf("Inter value %s\n", insertCmd.args[insertCmd.numArgs]);
              }
       | OPTION { insertCmd.args[++insertCmd.numArgs] = $1;
       			  //printf("Inter option %s\n", insertCmd.args[insertCmd.numArgs]);
       			};

pipe: PIPE { //printf("PIPE |\n"); 
              ++numPipes;
              reInitCurCmd(true);
              insertCmd.name = "|"; 
              insertCmd.args[0] = insertCmd.name;
        	    if(swapping == true){
          		  cmdTab[tempNumTabCmds++] = insertCmd;
                lastPipePos = tempNumTabCmds - 2;
          	  }else{
          		  cmdTab[numTabCmds++] = insertCmd;
                lastPipePos = numTabCmds - 1;
          	  } 
	 };

%%

/*
COMMAND OPTION VALUE PIPE COMMAND VALUE PIPE COMMAND

COMMAND axis
        axis inter
             OPTION
        axis inter
             VALUE
        axis inter
             PIPE
        axis
        COMMAND
*/
