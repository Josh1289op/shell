%{
#include <stdio.h>
#include "test.tab.h"
%}

%%

[ \t]+				;

stop    			return STOP;
start   			return START;
cd  				return CD;
ls					return LS;

[0-9]+       		{ 
						yylval.d = atoi(yytext); 
						return NUMBER; 
					}
[a-zA-Z/][a-zA-Z0-9/]*  	{ 
						yylval.a = yytext; 
						return WORD; 
					}
[\n]+				return EOL;
%%
