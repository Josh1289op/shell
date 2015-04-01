%{
#include <stdio.h>
#include "test.tab.h"
%}

%%
stop    			return STOP;
start   			return START;
[0123456789]+          	return NUMBER;
[a-zA-Z][a-zA-Z]*  	return WORD;
%%
