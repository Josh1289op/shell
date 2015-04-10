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

				while(++cmdTabPos <= numTabCmds){
					processCommand();
					reInitCurCmd();
					curCmd = &cmdTab[cmdTabPos];
				}
				break;
		}
		reInitCurCmd();
	}
}

void init(char ** envp){
  	//yydebug=1;
  	environment = envp;
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

	numTabCmds = 0;
	cmdTabPos = 0;
  	insertCmd.name = "empty"; 
	insertCmd.args[0] = insertCmd.name;
	cmdTab[numTabCmds] = insertCmd;
  	insertCmd.numArgs = 0;
	insertCmd.wait = true;
  	cmdTab[0] = insertCmd;
	
	numTabAls = 0;
	curAls = &alsTab[0];
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
	printf("%sasShell:%s%s%s>%s", KCYN, KGRY, cwd, KCYN, KNRM);
}

int getCommand(){
	init_Scanner_Parser();
	if (yyparse()){
		return understand_errors();
	} else if (strcmp(curCmd->name,"bye") == 0) {
		return BYE;
	} else {
		setBuiltins();
		return OK;
	}
}

void init_Scanner_Parser(){
	get_curr_dir();
	home = getenv("HOME");
	numTabCmds = 0;
	cmdTabPos = 0;
	//grab the next command from the table
	curCmd = &cmdTab[0];
}

//call this everytime you finish with the command you are currently working on
//this is needed for alias, pipes, and io redirect that have more than one command in a line
void reInitCurCmd() {
	//clear out the values of the cmd you just finished with
	curCmd->name = NULL;
	int i = 0;
	for(i; i != curCmd->numArgs + 1; ++i){
		curCmd->args[i] = NULL;
	}
	curCmd->numArgs = 0;
	curCmd->isBuiltin = false;
	curCmd->wait = true;
	
	insertCmd.name = NULL;
	for(i = 0; i != insertCmd.numArgs + 1; ++i){
		insertCmd.args[i] = NULL;
	}
	insertCmd.numArgs = 0;
	insertCmd.isBuiltin = false;
	insertCmd.wait = true;
}

void setBuiltins(){
	if(	  strcmp(curCmd->name,"cd") == 0
		|| strcmp(curCmd->name,"alias") == 0
		|| strcmp(curCmd->name,"unalias") == 0
		|| strcmp(curCmd->name,"printenv") == 0
		|| strcmp(curCmd->name,"getenv") == 0
		|| strcmp(curCmd->name,"setenv") == 0
		|| strcmp(curCmd->name,"unsetenv") == 0
		|| strcmp(curCmd->name,"exit") == 0) 
	{
		curCmd->isBuiltin = true;
		printf("isBuiltin = true\n");
	}
}

void processCommand(){
	if (curCmd->isBuiltin) {
		do_it();		// run built-in commands – no fork
						// no exec; only your code + Unix
						//system calls.
	} else if(strcmp(curCmd->name,"empty")) { //if the input is not empty, then execute the command
		shouldWait();
		execute_it();	// execute general commands
						//using fork and exec
	}
}

void shouldWait(){
	if(curCmd->numArgs > 0 && strcmp(curCmd->args[curCmd->numArgs],"&") == 0){
		curCmd->wait = false;
		curCmd->args[curCmd->numArgs] = NULL;
	}
}

int do_it() {
  	if(!isCommandValue){
	    printf("Command = %s\n", curCmd->name);
	    if(strcmp(curCmd->name,"cd") == 0){
			char * garbage;
			changeDirectory(true, garbage);
	    } else if(strcmp(curCmd->name, "printenv") == 0){
	      	char** env;
			for (env = environment; *env != 0; env++) {
			char* thisEnv = *env;
			printf("%s\n", thisEnv);
			}
	    } else if(strcmp(curCmd->name, "alias") == 0){
	    	int i;
		als * alsPointer = &alsTab[0];
		if(numTabAls == 0){
			printf("NO ALIASES FOUND!\n");
			return OK;		
		}
		for(i = 0; i < numTabAls; ++i){
			printf("%s = %s\n", alsPointer->alsName, alsPointer->alsValue);
			++alsPointer;		
		} 
	    } else if(strcmp(curCmd->name, "exit") == 0){
	      	exit(0);
	    }
  	} else {
	    isCommandValue = false;
	    printf("Command Value = %s %s %s %s\n", curCmd->name, curCmd->args[1], curCmd->args[2], curCmd->args[3]);
	    if(strcmp(curCmd->name,"cd") == 0){
	      	changeDirectory(false, curCmd->args[1]);
	    }else if(strcmp(curCmd->name, "setenv") == 0){
	      	//setenv variable value
	      	setenv(curCmd->args[1], curCmd->args[2], 1);
	      	run_getenv(curCmd->args[1]);
	    } else if(strcmp(curCmd->name, "unsetenv") == 0){
	      	unsetenv(curCmd->args[1]);
	    }else if(strcmp(curCmd->name, "getenv") == 0){
	      	//get a variable value
	      	run_getenv(curCmd->args[1]);
	    }else if(strcmp(curCmd->name, "alias") == 0){
		if(curCmd->args[1] == NULL || curCmd->args[2] == curCmd->args[2]){
			//return Error
		}
		printf("Setting Alias: %s = %s\n", curCmd->args[1], curCmd->args[2]);
		++numTabAls;
		curAls->alsName = curCmd->args[1];			
		curAls->alsValue = curCmd->args[2];
		++curAls;	
	    }
  	}
}

void execute_it(){
	//CHECK SLIDES FOR MORE CODE EXAMPLES ON THIS METHOD/////////////////////////////

	// Handle  command execution, pipelining, i/o redirection, and background processing.
	// Utilize a command table whose components are plugged in during parsing by yacc.
	
	int status; int err = 0;
	switch(pid = fork()) {
		case 0:
			//execlp("ls", "ls",(char *) NULL );   execlp("ls", "ls", "-l", (char *) NULL );
			execvp(curCmd->name, curCmd->args);
			if(status){
				printf("%s: command not recognized.\n", curCmd->name);
				exit(0);
			}
			break;

		default:
			if(curCmd->wait){
    			waitpid(pid, &status, 0);
    		}
    		break;
	}
}


int understand_errors(){
	//printf("understand_errors()\n");
	printf("command recieved: \"%s\"\n", insertCmd.name);
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
		fprintf(stdout, "directory %s/%s not found\n", get_curr_dir(), curCmd->args[1]);
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
