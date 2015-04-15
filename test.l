%{
#include <stdio.h>
#ifndef YYSTYPE
#define YYSTYPE char*
#endif
#include "test.tab.h"


int line_num = 1;
%}
/* ADDED THESE FOR I/O REDIRECTION*/
%%
"\$\{" {yylval = strdup(yytext); return OVAR; }
"\}" {yylval = strdup(yytext); return CVAR; }

"|" 	return PIPE;
">" 	return GT;
">>" 	return DGT;
"<" 	return LT;
"2>" 	return ERROR_CMD;

bye|cd|exit|alias|setenv|unsetenv|getenv|printenv    { yylval = strdup(yytext); return COMMAND; }

\~.* { 	yylval = strdup(yytext);	
	++yylval;
	char * fucker;
	fucker = getenv("HOME");
	char * lmao;
	strcpy(lmao, fucker);	
	concatenate_string(lmao, yylval);
	yylval = calloc(strlen(yytext)-1, sizeof(char));
	yylval = lmao;
	return VALUE; }

[-A-Za-z0-9\/\.\:\~]+ { yylval = strdup(yytext); return VALUE; }

\"[^\"]*\"    { //grab the info between the quotes and return as a value
			yylval = strdup(yytext); 
			yylval = calloc(strlen(yytext)-1, sizeof(char));
			strncpy(yylval, &yytext[1], strlen(yytext-2));
			yylval[strlen(yytext)-2] = '\0';
			//printf("between quotes = '%s'\n", yylval);
			return VALUE; }

-[A-Za-z0-9]+|&   { yylval = strdup(yytext); return OPTION; }

[ \t]   /* ignore whitespace */ ;

\n { return EOL; }

%%
