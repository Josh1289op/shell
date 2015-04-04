%{
#include <stdio.h>
#include "test.tab.h"
extern YYSTYPE yylval;


int line_num = 1;
%}

%%

ls|cd|bye|setenv|unsetenv|getenv|printenv    { yylval = strdup(yytext); return COMMAND; }

<<<<<<< HEAD
[A-Za-z0-9\/\.]+    { yylval = strdup(yytext); return VALUE; }
=======
[A-Za-z0-9/]+    { yylval = strdup(yytext); return VALUE; }
>>>>>>> 525d0946e1e0f1c57d2fa36c996ee2bd8721e127

-[A-Za-z0-9]+   { yylval = strdup(yytext); return OPTION; }

[ \t]   /* ignore whitespace */ ;

\n { return EOL; }

%%
