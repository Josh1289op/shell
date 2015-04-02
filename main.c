#include "global.h"

void main() {
	cd = 0;
    printf("\t TEST \n");
	while(1){
		printf("\n&:");
		yyparse();
		if(cd == 1){
			curr_dir();
			printf(changeDirectory);
			cd = 0;
		}
	}
}

void curr_dir() {
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL){
	   fprintf(stdout, "Current working dir: %s", cwd);
	}
	else{
	   perror("getcwd() error");
	}
}