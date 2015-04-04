#include "global.h"


extern int yydebug;

void main(int argc, char **argv, char** environ) {

  init(environ);
	while(1){
		prompt();
		int CMD = getCommand();
    	printf("CMD Value = %d\n", CMD);
		switch(CMD){
			case BYE:
				exit(0);
			case ERROR:
				handle_errors(); break;
			case OK:
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
  int isCommand = 0;
  int isCommandValue = 0;
  environmentcount = 0;
  char** env;
  for (env = environment; *env != 0; env++)
  {
    char* thisEnv = *env;
    ++environmentcount;
  }
  printf("%d", environmentcount);
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
	printf("%s\nasShell:%s%s%s>%s", KCYN, KGRY, cwd, KCYN, KNRM);
}

int getCommand(){
	init_Scanner_Parser();
	if (yyparse())
		return understand_errors();
	else if (builtin == Stop) {
		printf("%s", word);
		return BYE;
	} else {
		return OK;
	}
}

void init_Scanner_Parser(){
	get_curr_dir();
	home = getenv("HOME");
	int cd = 0;
	char* word = "";
	char cwd[1024] = "";
	int builtin = -1;
	int isBuiltin = true;
}

void processCommand(){
	if (isBuiltin) {
		do_it();		// run built-in commands – no fork
						// no exec; only your code + Unix
						//system calls.
	} else {
		execute_it();	// execute general commands
						//using fork and exec
	}
}

void do_it() {
  if(!isCommandValue){
    isCommand = 0;
    printf("Command = %s\n", cmd);
    if(strcmp(cmd,"cd") == 0){
      char * garbage;
      changeDirectory(1, garbage);
    }else if(strcmp(cmd, "printenv") == 0){
      char** env;
      for (env = environment; *env != 0; env++)
      {
        char* thisEnv = *env;
        printf("%s\n", thisEnv);
      }
    }else if(strcmp(cmd, "bye") == 0){
      exit(0);
    }
  }else {
    isCommandValue = 0;
    printf("Command Value = %s %s %s %s\n", cmd, value[0], value[1], value[2]);
    if(strcmp(cmd,"cd") == 0){
      changeDirectory(0, value[0]);
    }else if(strcmp(cmd, "setenv") == 0){
      //setenv variable value
      setenv (value[0], value[1], 1);
      run_getenv(value[0]);

    }else if(strcmp(cmd, "unsetenv") == 0){
      unsetenv(value[0]);
    }else if(strcmp(cmd, "getenv") == 0){
      //get a variable value
      run_getenv(value[0]);
    }
  }
}

void execute_it(){
	//CHECK SLIDES FOR MORE CODE EXAMPLES ON THIS METHOD/////////////////////////////

	// Handle  command execution, pipelining, i/o redirection, and background processing.
	// Utilize a command table whose components are plugged in during parsing by yacc.

	/*
	 * Check Command Accessability and Executability
	*/


	/*
	 * Check io file existence in case of io-redirection.
	*/

	//Build up the pipeline (create and set up pipe end points (using pipe, dup)
	//Process background
}


int understand_errors(){
	printf("understand_errors()\n");
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
		char *cwd = "";
		perror("no current working directory");
	} else {
		return cwd;
	}
	return cwd;
}

void changeDirectory(int goHome, char * dir) {
	if(goHome){
		cd = chdir(home);
	} else {
		cd = chdir(dir);
	}

	if(cd == -1){
		fprintf(stdout, "directory %s/%s not found\n", get_curr_dir(), word);
	}else{
		//execlp("ls", "ls", "-l",(char *) NULL );
		//printf("chdir(%s) succeeded", word);
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
