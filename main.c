#include "global.h"


extern int yydebug;

void main(int argc, char **argv, char** environ) {

  	init(environ);

  	//is the input from a file?
  	cmdFromFile(argv[1]);


	runShell();
}

void runShell() {

	do {
		if(runPrompt) prompt();
		int CMD = getCommand();
		if(runPrompt)printf("----------\n");
		if(runPrompt)printTable();
		CMD = aliasChecker();
		if(runPrompt)printf("----------\n");
		if(runPrompt)printTable();
		if(runPrompt)printf("----------\n");
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
	} while(runPrompt);
}

int cmdFromFile(char* inputFileName) {
	if(inputFileName == NULL) return false;
	FILE *fin;
	char *mode = "r";

	fin = fopen(inputFileName, mode);
	
	if (fin == NULL) {
		errorCode = 8;
		hasErrors = true;
		handle_errors();
 	 	return false;
	}

	runPrompt = false;
    while (fgets (parseInput, 1024, fin)) {
    	printf("\n%sasShell:%s%s%s", KCYN, KGRY, parseInput, KNRM);
        YY_BUFFER_STATE my_string_buffer = yy_scan_string(parseInput);
		runShell();
		yy_delete_buffer(my_string_buffer);
		yylex_destroy();
    }
    fclose (fin);
    runPrompt = true;
	return true;
}

int aliasChecker(){
	//printf("Number of Aliases: %d\n", numTabAls);
	lastAliasUsed = -1;
	int pos = 0; int argPos = 0; int aliasPos = 0;
	for(pos; pos < numTabCmds; ++pos){
		tempNumTabCmds = numTabCmds;
		for(aliasPos = 0; aliasPos < numTabAls; ++aliasPos){
			if(strcmp(alsTab[aliasPos].alsName,cmdTab[pos].name) == 0) {
				swapping = true;
				if(alsTab[aliasPos].used && pos == lastAliasUsed ) {
					errorCode = 4;
					hasErrors = true;
					swapping = false;
					return ERROR;
				}else if(alsTab[aliasPos].used) {
					int tempAliasPos = 0;
					for(tempAliasPos; tempAliasPos < numTabAls; ++tempAliasPos){
						alsTab[tempAliasPos].used = false;
					}
				}
				lastAliasUsed = pos;
				//printTable();
				reYYPARSEString = alsTab[aliasPos].alsValue;
				printf("%s", reYYPARSEString);
				int argPosRecat;
				for(argPosRecat = 1; argPosRecat < cmdTab[pos].numArgs; ++argPosRecat){
					printf("%d", argPosRecat);
					concatenate_string(reYYPARSEString, " ");
					concatenate_string(reYYPARSEString, cmdTab[pos].args[argPosRecat]);
				}
				printf("%s", reYYPARSEString);

				YY_BUFFER_STATE my_string_buffer = yy_scan_string(reYYPARSEString);
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
				printf("After - Entries: %d & Temp Entries: %d\n",numTabCmds, tempNumTabCmds);
				printTempTable(tempCmdTab);
				int j = 0;
				int tempAliasPos = pos;
				//Move all the new cmds where they go in the table
				for(j = numTabCmds; j < tempNumTabCmds; ++j){
					tempCmdTab[tempAliasPos++] = cmdTab[j];
				}
				printTempTable(tempCmdTab);
				//Move the rest of the cmds below the new cmds
				for(i; i < numTabCmds; ++i){
					tempCmdTab[tempAliasPos++] = cmdTab[i];
				}
				printTempTable(tempCmdTab);
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

	numTabCmds = 0;
	cmdTabPos = 0;
  	insertCmd.name = "empty"; 
	insertCmd.args[0] = insertCmd.name;
	cmdTab[numTabCmds] = insertCmd;
  	insertCmd.numArgs = 0;
	insertCmd.wait = true;
  	cmdTab[0] = insertCmd;
	
	runPrompt = true;
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
	//for(i; i < numPipes; ++i){
	//	pipeFd[i] = NULL;
	//}

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
				errorCode = 7;
				hasErrors = true;
				return;

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

	int p;
	for(p=0; p < numPipes; ++p){
		if(pipe(pipeFd[p]) == -1){
			errorCode = 5;
			hasErrors = true;
			return;
		}
	}
	for(p = 0; p <= numPipes; ++p) {
		if(fork() == 0){
			//printf("numtabCmds: %d\n", numTabCmds);
			if(p == 0){
				//first cmd;
				//printf("First Command: %s\n", cmdTab[cmdTabPos - 1].name);
				close(OUTPUT);
				dup(pipeFd[p][OUTPUT]);
			}else if(p == numTabCmds - numPipes - 1){
				//last cmd
				//printf("Last Command: %s\n", cmdTab[cmdTabPos - 1].name);
				close(INPUT);
				dup(pipeFd[p - 1][INPUT]);
			}else{
				//every other command
				//printf("Command %d: %s\n", p, cmdTab[cmdTabPos - 1].name);
				dup2(pipeFd[p-1][0], STDIN_FILENO);
				dup2(pipeFd[p][1], STDOUT_FILENO);

			}	
			int pc;
			for(pc = 0; pc < numPipes; ++pc){
				close(pipeFd[pc][INPUT]);
				close(pipeFd[pc][OUTPUT]);
			}	
	
			execvp( cmdTab[cmdTabPos - 1].name, cmdTab[cmdTabPos - 1].args );
			errorCode = 6;
			hasErrors = true;
			curCmd = &cmdTab[cmdTabPos - 1];
			return;
		}else {
				cmdTabPos += 2;
		}
	}
	// close pipes
	int pc;
	for(pc = 0; pc < numPipes; ++pc){
		close(pipeFd[pc][INPUT]);
		close(pipeFd[pc][OUTPUT]);
	}	

	// wait once per pipe
	for(p = 0; p <= numPipes; ++p){
		wait(0);
	}
}



int understand_errors(){
	//printf("understand_errors()\n");
	printf("YYParse failed: command recieved: \"%s\"\n", insertCmd.name);
	return ERROR;
}

void handle_errors(){

	// Find out if error occurs in middle of command
	// That is, the command still has a “tail”
	// In this case you have to recover by “eating”
	// the rest of the command.
	// To do this: use yylex() directly.

	switch(errorCode){
		case 0:
			fprintf(stderr, "%sError: %sUnknown command %s\n%s", IRed, IRedU, curCmd->name,IOReset);
			break;
		case 1:
			fprintf(stderr, "%sError: %sThe \"%s\" command takes two arguments \n%s%sInput:%s %s %s %s\n%s", IRed, IRedU,  curCmd->name, IOReset, IRed, IRedU, curCmd->name, curCmd->args[1], curCmd->args[2], IOReset);
			break;
		case 2:
			fprintf(stderr, "%sError: %sThe \"%s\" command takes one argument \n%sInput:%s %s %s\n%s", IRed, IRedU, curCmd->name, IRed, IRedU, curCmd->name, curCmd->args[1], IOReset);
			break;
		case 3:
			fprintf(stderr, "%sError: %sThe \"%s\" command takes arguments and you supplied none. \nInput: %s \n%s", IRed, IRedU,curCmd->name, curCmd->name, IOReset);
			break;
		case 4:
			fprintf(stderr, "%sError: %sThe alias \"%s\" has a circular reference.\n%s", IRed, IRedU, curCmd->name, IOReset);
			break;
		case 5:
			fprintf(stderr, "%sPipe Error:\n%s",IRed,IOReset);
			break;
		case 6:
			fprintf(stderr, "%sError: %sUnknown command %s%s\n", IRed, IRedU, curCmd->name, IOReset);
			exit(1);
			break;
		case 7:
			//EXECVP EXIT ERROR;
			fprintf(stderr, "%sError: %sUnknown command %s\n%s", IRed, IRedU, curCmd->name,IOReset);
			exit(0);			
			break;
		case 8:
			fprintf(stderr, "%sError: %sInput file not found. \n%s", IRed, IRedU, IOReset);
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

void concatenate_string(char *original, char *add)
{
   while(*original)
      original++;
 
   while(*add)
   {
      *original = *add;
      add++;
      original++;
   }
   *original = '\0';
}

