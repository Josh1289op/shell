%{
#include <stdio.h>
#include "test.tab.h"
%}

%%
stop    			return STOP;
start   			return START;
cd  				return CD;
[0123456789]+       { 
						yylval.d = atoi(yytext); 
						return NUMBER; 
					}
[a-zA-Z][a-zA-Z]*  	{ 
						yylval.a = yytext; 
						return WORD; 
					}
%%
