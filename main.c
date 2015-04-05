#include "global.h"


extern int yydebug;

void main(int argc, char **argv, char** environ) {

  init(environ);
	while(1){
		prompt();
		int CMD = getCommand();
		switch(CMD){
			case BYE:
			printf("CMD case: BYE\n");
				exit(0);
			case ERROR:
				printf("CMD case: ERROR\n");
				handle_errors(); break;
			case OK:
				printf("CMD case: OK\n");
				processCommand(); break;
		}
	}
}

void init(char ** envp){
  	//yydebug=1;
  	environment = envp;
  	valuecount = 0;
	get_curr_dir();
	home = getenv("HOME");
	path = getenv("PATH");
  	int isCommandValue = false;
  	environmentcount = 0;
  	char** env;
  	for (env = environment; *env != 0; env++)
  	{
    	char* thisEnv = *env;
    	++environmentcount;
  	}
  	printf("%d\n", environmentcount);
	// init all variables.
	// define (allocate storage) for some var/tables
	// init all tables (e.g., alias table)
	// get PATH environment variable (use getenv())
	// get HOME env variable (also use getenv())
	// disable anything that can kill your shell.
	// (the shell should never die; only can be exit)
	// do anything you feel should be done as init
}

void prompt(){
	get_curr_dir();
  valuecount = 0;
	printf("%sasShell:%s%s%s>%s", KCYN, KGRY, cwd, KCYN, KNRM);
}

int getCommand(){
	init_Scanner_Parser();
	if (yyparse())
		return understand_errors();
	else if (strcmp(curCmd.name,"end") == 0) {
		return BYE;
	} else {
		setBuiltins();
		return OK;
	}
}

void init_Scanner_Parser(){
	get_curr_dir();
	home = getenv("HOME");
	curCmd.isBuiltin = false;
	curCmd.numOpts = 0;
	curCmd.wait = true;
}

void setBuiltins(){
	if(	  strcmp(curCmd.name,"cd") == 0
		|| strcmp(curCmd.name,"printenv") == 0
		|| strcmp(curCmd.name,"getenv") == 0
		|| strcmp(curCmd.name,"setenv") == 0
		|| strcmp(curCmd.name,"unsetenv") == 0
		|| strcmp(curCmd.name,"exit") == 0) 
	{
		curCmd.isBuiltin = true;
		printf("isBuiltin = true\n");
	}
}

void processCommand(){
	if (curCmd.isBuiltin) {
		do_it();		// run built-in commands – no fork
						// no exec; only your code + Unix
						//system calls.
	} else {
		shouldWait();
		execute_it();	// execute general commands
						//using fork and exec
	}
}

void shouldWait(){
	if(strcmp(curCmd.opt[curCmd.numOpts],"&") == 0){
		curCmd.wait = false;
		curCmd.opt[curCmd.numOpts] = NULL;
	}
}

void do_it() {
  	if(!isCommandValue){
	    printf("Command = %s\n", curCmd.name);
	    if(strcmp(curCmd.name,"cd") == 0){
			char * garbage;
			changeDirectory(true, garbage);
	    } else if(strcmp(curCmd.name, "printenv") == 0){
	      	char** env;
			for (env = environment; *env != 0; env++) {
			char* thisEnv = *env;
			printf("%s\n", thisEnv);
			}
	    } else if(strcmp(curCmd.name, "exit") == 0){
	      	exit(0);
	    }
  	} else {
	    isCommandValue = false;
	    printf("Command Value = %s %s %s %s\n", curCmd.name, value[0], value[1], value[2]);
	    if(strcmp(curCmd.name,"cd") == 0){
	      	changeDirectory(false, value[0]);
	    }else if(strcmp(curCmd.name, "setenv") == 0){
	      	//setenv variable value
	      	setenv(value[0], value[1], 1);
	      	run_getenv(value[0]);

	    } else if(strcmp(curCmd.name, "unsetenv") == 0){
	      	unsetenv(value[0]);
	    }else if(strcmp(curCmd.name, "getenv") == 0){
	      	//get a variable value
	      	run_getenv(value[0]);
	    }
  	}
}

void execute_it(){
	//CHECK SLIDES FOR MORE CODE EXAMPLES ON THIS METHOD/////////////////////////////

	// Handle  command execution, pipelining, i/o redirection, and background processing.
	// Utilize a command table whose components are plugged in during parsing by yacc.
	
	int status;
	switch(pid = fork()) {
		case 0:
			if(!isCommandValue){
			    printf("Command = %s\n", curCmd.name);
			    if(strcmp(curCmd.name,"ls") == 0){
					//execlp("ls", "ls",(char *) NULL );
					curCmd.opt[0] = "ls";
					execvp("ls", curCmd.opt);
			    } else if(strcmp(curCmd.name, "x") == 0){
			      	
			    } else if(strcmp(curCmd.name, "x") == 0){
			      	
			    }
		  	} else {
			    isCommandValue = false;
			    printf("Command Value = %s %s %s %s\n", curCmd.name, value[0], value[1], value[2]);
			    if(strcmp(curCmd.name,"ls") == 0){
					//execlp("ls", "ls", "-l",(char *) NULL );
					curCmd.opt[0] = "ls";
					execvp("ls", curCmd.opt);
			    }else if(strcmp(curCmd.name, "x") == 0){
			      	
			    } else if(strcmp(curCmd.name, "x") == 0){
			      	
			    }else if(strcmp(curCmd.name, "x") == 0){
			      	

			    }
		  	} break;

		default:
			if(curCmd.wait){
    			waitpid(pid, &status, 0);
    		}
    		break;
	}
}


int understand_errors(){
	//printf("understand_errors()\n");
	printf("command recieved: \"%s\"\n", curCmd.name);
	return ERROR;
}

void handle_errors(){
	printf("handle_errors()\n");
	// Find out if error occurs in middle of command
	// That is, the command still has a “tail”
	// In this case you have to recover by “eating”
	// the rest of the command.
	// To do this: use yylex() directly.
}






char* get_curr_dir() {
	if (getcwd(cwd, sizeof(cwd)) == NULL){
		perror("no current working directory");
	} else {
		return cwd;
	}
	return cwd;
}

void changeDirectory(int goHome, char * dir) {
	int err = 0;
	if(goHome){
		err = chdir(home);
	} else {
		err = chdir(dir);
	}

	if(err == -1){
		fprintf(stdout, "directory %s/%s not found\n", get_curr_dir(), value[0]);
	}	
}

void run_getenv (char * name)
{
    char * value;

    value = getenv (name);
    if (! value) {
        printf ("'%s' is not set.\n", name);
    }
    else {
        printf ("%s = %s\n", name, value);
    }
}
