#include "global.h"


extern int yydebug;

void main(int argc, char **argv, char** environ) {

  init(environ);
	while(1){
		prompt();
		int CMD = getCommand();
		printf("----------\n");
		printTable();
		CMD = aliasChecker();
		printf("----------\n");
		printTable();
		printf("----------\n");
		switch(CMD){
			case BYE:
				printf("CMD case: BYE\n");
				exit(0);
			case ERROR:
				//printf("CMD case: ERROR\n");
				handle_errors(); break;
			case OK:
				//printf("CMD case: OK\n");

				while(cmdTabPos++ < numTabCmds){
					//printf("curCmd: %s %s, %s\n", curCmd->name, curCmd->args[1], curCmd->args[2]);
					processCommand();
					if(hasErrors) {					
						handle_errors();
					}
					reInitCurCmd(false);
					curCmd = &cmdTab[cmdTabPos];
				}
				
				break;
		}
		reInitCurCmd(false);
	}
}

int aliasChecker(){
	

	//printf("Number of Aliases: %d\n", numTabAls);
	int pos = 0; int argPos = 0; int aliasPos = 0;
	for(pos; pos < numTabCmds; ++pos){
		tempNumTabCmds = numTabCmds;
		for(aliasPos = 0; aliasPos < numTabAls; ++aliasPos){
			if(strcmp(alsTab[aliasPos].alsName,cmdTab[pos].name) == 0) {
				swapping = true;
				//printf("Found Alias: %s = %s \n", alsTab[aliasPos].alsName, alsTab[aliasPos].alsValue);
				if(alsTab[aliasPos].used) {
					errorCode = 4;
					hasErrors = true;
					return ERROR;
				}
				
				//printf("\nBefore - Entries: %d & Temp Entries: %d\n",numTabCmds, tempNumTabCmds);
				//printTable();
				my_string_buffer = yy_scan_string(alsTab[aliasPos].alsValue);
    				int my_parse_result  = yyparse();
    				yy_delete_buffer(my_string_buffer);
				yylex_destroy();
				//Adding support for multiple cmds in one alias
				int i = 0;
				cmd tempCmdTab[MAXCMD];
				//Get all of the cmds before alias and put them in temp table
				for(i = 0; i < pos + 1; ++i){
					tempCmdTab[i] = cmdTab[i];
				}
				//printTempTable(tempCmdTab);
				int j = 0;
				int tempAliasPos = pos;
				//Move all the new cmds where they go in the table
				for(j = numTabCmds; j < tempNumTabCmds; ++j){
					tempCmdTab[tempAliasPos++] = cmdTab[j];
				}
				//printTempTable(tempCmdTab);
				//Move the rest of the cmds below the new cmds
				for(i; i < numTabCmds; ++i){
					tempCmdTab[tempAliasPos++] = cmdTab[i];
				}
				//printTempTable(tempCmdTab);
				//Copy the new table over to the old table
				reInitCurCmd(false);
				for(i = 0; i < tempNumTabCmds - 1; ++i){
					cmdTab[i] = tempCmdTab[i];				
				}
				numTabCmds = i;
				//printTable();
				//printf("tempNumTabCmds: %d\n", tempNumTabCmds);					
				//printf("After - Entries: %d & Temp Entries: %d\n",numTabCmds, tempNumTabCmds);
				alsTab[aliasPos].used = true;
				swapping = false;
				pos = -1;
				break;			
			}	
		}
		if(pos == -1){
			aliasPos = 0;
			continue;
		}
		
	}
	//printf("finished alias checker\n");
	if(strcmp(curCmd->name,"bye") == 0) return BYE;
	else return OK;
	
}

void printTable(){
	int pos = 0; int argPos = 0;
	for(pos; pos < numTabCmds; ++pos){
		printf("%d: ", pos);
		for(argPos; argPos <= cmdTab[pos].numArgs + 1; ++argPos){
			printf("%s ",  cmdTab[pos].args[argPos]);
		}
		printf("%d\n", cmdTab[pos].isCommandValue);
		argPos = 0;
	}
}
void printTempTable(cmd * tempTable){
	printf("?****** TEMP TABLE *****?\n");
	int pos = 0; int argPos = 0;
	for(pos; pos < numTabCmds; ++pos){
		printf("%d: ", pos);
		for(argPos; argPos <= tempTable[pos].numArgs + 1; ++argPos){
			printf("%s ",  tempTable[pos].args[argPos]);
		}
		printf("%d\n", tempTable[pos].isCommandValue);
		argPos = 0;
	}
	printf("?****** END TEMP TABLE *****?\n");
}

void init(char ** envp){
  	//yydebug=1;
  	environment = envp;
	get_curr_dir();
	home = getenv("HOME");
	path = getenv("PATH");
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


	swapping = false;
	hasErrors = false;
	errorCode = 0;
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
		//printf("bye returned in getCommand\n");
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
	
	//grab the first command from the table
	curCmd = &cmdTab[0];
	int aliasPos = 0;
	for(aliasPos; aliasPos < numTabAls; ++aliasPos){
		alsTab[aliasPos].used = false;
	}
	int i = 0;
	for(i; i < numPipes; ++i){
		pipeFds[i] = NULL;
	}

	numPipes = 0; pipePos = 0; lastPipePos = 0;	pipePidPos = 0; hasErrors = false; errorCode = 0;

}

//call this everytime you finish with the command you are currently working on
//this is needed for alias, pipes, and io redirect that have more than one command in a line
void reInitCurCmd(int fromYacc) {
	//clear out the values of the cmd you just finished with
	if(!fromYacc){
		curCmd->name = NULL;
		int i = 0;
		for(i; i != curCmd->numArgs + 1; ++i){
			curCmd->args[i] = NULL;
		}
		curCmd->numArgs = 0;
		curCmd->isBuiltin = false;
		curCmd->wait = true;
		curCmd->isCommandValue = false;
	}
	
	insertCmd.name = NULL;
	int i = 0;
	for(i; i != insertCmd.numArgs + 1; ++i){
		insertCmd.args[i] = NULL;
	}
	insertCmd.numArgs = 0;
	insertCmd.isBuiltin = false;
	insertCmd.wait = true;
	insertCmd.isCommandValue = false;
}

void setBuiltins(){
	int counter = 0;
	while(counter < numTabCmds){
		cmd *temp = &cmdTab[counter++];

		if(	  strcmp(temp->name,"cd") == 0
			|| strcmp(curCmd->name,"alias") == 0
			|| strcmp(curCmd->name,"unalias") == 0
			|| strcmp(temp->name,"printenv") == 0
			|| strcmp(temp->name,"getenv") == 0
			|| strcmp(temp->name,"setenv") == 0
			|| strcmp(temp->name,"unsetenv") == 0
			|| strcmp(temp->name,"|") == 0
			|| strcmp(temp->name,"exit") == 0) 
		{
			temp->isBuiltin = true;
			//printf("isBuiltin = true\n");
		}
		//else printf("isBuiltin = false\n");
	}
}

void processCommand(){
	if (curCmd->isBuiltin) {
		do_it();		// run built-in commands – no fork
						// no exec; only your code + Unix
						//system calls.
	} else if(strcmp(curCmd->name,"empty")) { //if the input is not empty, then execute the command
		shouldWait();
		//printf("numPipes: %d\n", numPipes);
		if(numPipes > 0){
			execute_pipe();		//execute commands that have pipes
		} else {
			execute_command();	// execute general commands using fork and exec
		}
	}
}

void shouldWait(){
	if(curCmd->numArgs > 0 && strcmp(curCmd->args[curCmd->numArgs],"&") == 0){
		curCmd->wait = false;
		curCmd->args[curCmd->numArgs] = NULL;
	}
}


int do_it() {
	//printf("do_it curCmd->name: %s\n", curCmd->name);
	//printf("%d", curCmd->isCommandValue);
  	if(!curCmd->isCommandValue){
	    //printf("Command = %s\n", curCmd->name);
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
	    } else if(strcmp(curCmd->name, "setenv") == 0 || strcmp(curCmd->name, "unsetenv") == 0 || strcmp(curCmd->name, "getenv") == 0){
		errorCode = 3;
		hasErrors = true;
		return ERROR;
	    }
  	} else {
	    curCmd->isCommandValue = false;
	    //printf("Command Value = %s %s %s %s\n", curCmd->name, curCmd->args[1], curCmd->args[2], curCmd->args[3]);
	    if(strcmp(curCmd->name,"cd") == 0){
	      	changeDirectory(false, curCmd->args[1]);
	    }else if(strcmp(curCmd->name, "setenv") == 0){
		if(curCmd->args[1] == NULL || curCmd->args[2] == NULL){
			errorCode = 1;
			hasErrors = true;
			return ERROR;
		}
	      	//setenv variable value
	      	setenv(curCmd->args[1], curCmd->args[2], 1);
	      	run_getenv(curCmd->args[1]);
	    } else if(strcmp(curCmd->name, "unsetenv") == 0){
		if(curCmd->args[1] == NULL || curCmd->args[2] == NULL){
			errorCode = 2;
			hasErrors = true;
			return ERROR;
		}
	      	unsetenv(curCmd->args[1]);
	    }else if(strcmp(curCmd->name, "getenv") == 0){
		if(curCmd->args[1] == NULL){
			errorCode = 2;
			hasErrors = true;
			return ERROR;
		}
	      	//get a variable value
	      	run_getenv(curCmd->args[1]);
	    } else if(strcmp(curCmd->name, "|") == 0){
	      	//pipes
	      	//printf("do_pipe: %s\n", curCmd->name);
	    } else if(strcmp(curCmd->name, "alias") == 0){
		
		if(curCmd->args[1] == NULL || curCmd->args[2] == NULL){
			errorCode = 1;
			hasErrors = true;
			return ERROR;
		}
		int aliasPos = 0;
		//Checking if the alias already exist
		for(aliasPos; aliasPos < numTabAls; ++aliasPos){
			if(strcmp(curCmd->args[1],alsTab[aliasPos].alsName) == 0){
				alsTab[aliasPos].alsValue = curCmd->args[2];
				return OK;
			} 
		}
		printf("Setting Alias: %s = %s\n", curCmd->args[1], curCmd->args[2]);
		++numTabAls;
		curAls->used = false;
		curAls->alsName = curCmd->args[1];			
		curAls->alsValue = curCmd->args[2];
		++curAls;	
	   }
  	}
}

void execute_command(){
	//CHECK SLIDES FOR MORE CODE EXAMPLES ON THIS METHOD/////////////////////////////

	// Handle  command execution, pipelining, i/o redirection, and background processing.
	// Utilize a command table whose components are plugged in during parsing by yacc.

	int status;
	switch(pids = fork()) {
		case 0:
			//execlp("ls", "ls",(char *) NULL );   execlp("ls", "ls", "-l", (char *) NULL );
			status = execvp(curCmd->name, curCmd->args);
			if(status){
				printf("%s: command not recognized.\n", curCmd->name);
				exit(0);
			}
			break;

		default:
			if(curCmd->wait){
    			waitpid(pids, &status, 0);
    		}
    		break;
	}
}

void execute_pipe(){
	//printf("inside execute pipe\n");

	if(pipeFds[pipePos] == NULL){
		int i = 0;
		for(i; i <= numPipes; ++i){
			int fd[2];
			pipeFds[i] = fd;
			pipe(pipeFds[i]);
		}
	}
	

	//printf("numPipes: %d\n", numPipes);
		
	switch (pid[pipePidPos++] = fork()) {
		case 0: // child 
			if(cmdTabPos == 1){ 				//first command before pipe; uses stdIn
				//dup2(pipeFds[pipePos][1], STDOUT_FILENO);	// this end of the pipe becomes the standard output 
				//close(pipeFds[pipePos][0]); 		// this process don't need the other end 
				close(1);
				int temp = dup(pipeFds[pipePos][1]);
			} else if (cmdTabPos == lastPipePos + 2) {	//last command after all pipes; uses stdOut
				dup2(pipeFds[pipePos][0], STDIN_FILENO);	// this end of the pipe becomes the standard input 
				//close(pipeFds[pipePos][1]);		// this process doesn't need the other end 
				close(pipeFds[pipePos][0]);
				//int temp = dup(pipeFds[1][0]);
			} else {							//cmds between pipes; change in and out;
				dup2(pipeFds[pipePos][0], STDIN_FILENO);	// this end of the pipe becomes the standard input
				dup2(pipeFds[++pipePos][1], STDOUT_FILENO);	// this end of the pipe becomes the standard output 
				//int temp = dup(pipeFds[0][0]);
				//temp = dup(pipeFds[1][1]);
			}

			int k;
			for(k=0; k <= numPipes ; ++k){
				close(pipeFds[k][0]); close(pipeFds[k][1]); 	// this is important! close both file descriptors on the pipe 
			}

			pipeStatus = execvp(curCmd->name, curCmd->args);	// run the command 
			
			if(pipeStatus){
				fprintf(stderr, "inside fork: process %d exits with %d\n", pid[pipePidPos - 1], pipeStatus);
				exit(1);
			}
			perror(curCmd->name);	// it failed!

		default: // parent does nothing 
			break;

		case -1:
			printf("fork error\n");
			perror("fork");
			exit(1);
	}

	if (cmdTabPos == lastPipePos + 2) {
		int k;
		for(k=0; k <= numPipes ; ++k){
			close(pipeFds[k][0]); close(pipeFds[k][1]); 	// this is important! close both file descriptors on the pipe 
		}
		for(k=0; k <= numPipes ; ++k){
			waitpid(pid[k], &pipeStatus, 0);
			fprintf(stderr, "process %d exits with %d\n", pid[k], pipeStatus);
		}
		
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

	switch(errorCode){
		case 0:
			fprintf(stderr, "Unknown Error: %s\n", curCmd->name);
			break;
		case 1:
			fprintf(stderr, "Error: The \"%s\" command takes two arguments \nInput: %s %s %s \n", curCmd->name, curCmd->name, curCmd->args[1], curCmd->args[2]);
			break;
		case 2:
			fprintf(stderr, "Error: The \"%s\" command takes one argument \nInput: %s %s \n", curCmd->name,curCmd->name, curCmd->args[1]);
			break;
		case 3:
			fprintf(stderr, "Error: The \"%s\" command takes arguments and you supplied none. \nInput: %s \n", curCmd->name, curCmd->name);
			break;
		case 4:
			fprintf(stderr, "Error: The alias \"%s\" has a circular reference.\n", curCmd->name);
			break;
		case 5:
			fprintf(stderr, "Unknown Error: %s\n", curCmd->name);
			break;


	}

	errorCode = 0;
	hasErrors = false;
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

