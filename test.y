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

%token PIPE LEFT_OPEN RIGHT_OPEN ERROR_CMD COMMAND VALUE OPTION OVAR CVAR EOL 

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
      | OPTION      { insertCmd.name = $1; return 1; }
      | VALUE OVAR VALUE CVAR { printf("Found Variable: %s = %s\n", $3, getenv ($3)); 
			reYYPARSEString = $1;
			concatenate_string(reYYPARSEString, " ");
			concatenate_string(reYYPARSEString, getenv($3));
			YY_BUFFER_STATE my_string_buffer = yy_scan_string(reYYPARSEString);
			int my_parse_result  = yyparse();
			yy_delete_buffer(my_string_buffer);
			yylex_destroy();
			YYACCEPT;
			};
      | OVAR VALUE CVAR { printf("Found Variable here: %s = %s\n", $2, getenv ($2));
			init(environment); 
			YY_BUFFER_STATE my_string_buffer = yy_scan_string(getenv($2));
			int my_parse_result  = yyparse();
			yy_delete_buffer(my_string_buffer);
			yylex_destroy();
			YYACCEPT;
			}
      | COMMAND VALUE OVAR VALUE CVAR { 
			reYYPARSEString = $1;
			concatenate_string(reYYPARSEString, " ");
			concatenate_string(reYYPARSEString, $2);
			concatenate_string(reYYPARSEString, " \"");
			concatenate_string(reYYPARSEString, $3);
			concatenate_string(reYYPARSEString, $4);
			concatenate_string(reYYPARSEString, $5);
			concatenate_string(reYYPARSEString, "\"");
			my_string_buffer = yy_scan_string(reYYPARSEString);
			int my_parse_result  = yyparse();
			yy_delete_buffer(my_string_buffer);
			yylex_destroy();
			YYACCEPT;
			};

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
