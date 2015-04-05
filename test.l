%{
#include <stdio.h>
#ifndef YYSTYPE
#define YYSTYPE char*
#endif
#include "test.tab.h"


int line_num = 1;
%}

%%

end|ls|cd|exit|setenv|unsetenv|getenv|printenv    { yylval = strdup(yytext); return COMMAND; }

[A-Za-z0-9\/\.]+    { yylval = strdup(yytext); return VALUE; }

-[A-Za-z0-9]+|&   { yylval = strdup(yytext); return OPTION; }

[ \t]   /* ignore whitespace */ ;

\n { return EOL; }

%%
