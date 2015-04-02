%{
#include <stdio.h>
#include "test.tab.h"

int line_num = 1;
%}

%%

[ \t]*				;
start   			return START;
cd  				return CD;
ls					return LS;

stop    			return STOP;
end    				return STOP;
exit    			return STOP;
halt    			return STOP;

[0-9]+       		{ yylval.d = atoi(yytext); return NUMBER; };
[a-zA-Z\/][a-zA-Z0-9\/]*[^\n]  	{ yylval.a = yytext; return WORD; };

\n				{ ++line_num; return EOL; }

%%
