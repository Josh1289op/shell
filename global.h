#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>         /* defines options flags */
#include <sys/types.h>     /* defines types used by sys/stat.h */
#include <sys/stat.h>      /* defines S_IREAD & S_IWRITE  */

#define KNRM  "\x1B[0m"
#define KGRY  "\x1B[37m"
#define KCYN  "\x1B[36m"


#define OK 0
#define BYE 1
#define ERROR -1
#define true 1
#define false 0

#define MAXCMD 100
#define MAXALIAS 100


#define READ 0
#define WRITE 1

typedef int YY_BUFFER_STATE;
YY_BUFFER_STATE my_string_buffer;

void init(char**);
void prompt();

void run_getenv (char * name);
int check_for_env(char * var);
int getCommand();
void processCommand();
int do_it();
void execute_command();
void handle_errors();
int understand_errors();
void init_Scanner_Parser();
void setBuiltins();
char* get_curr_dir();
void changeDirectory(int, char *);
void shouldWait();
void reInitCurCmd();
void execute_pipe();
int aliasChecker();
void printTable();


//Initialization 
typedef struct _CMD {
	char* name;
	char* args[MAXCMD];
	int numArgs;
	int isBuiltin;
	int wait;
	int isCommandValue;

	int inFd;
	int outFd;
	int errFd;
} cmd;

cmd cmdTab[MAXCMD];
cmd insertCmd;
cmd* curCmd;
int numTabCmds;
int cmdTabPos;

//Using these variables to keep track of cmdTab organization for swamping aliases
int swapping;
int tempNumTabCmds;


typedef struct _ALS {
	char * alsName;			//Alias Variable String
	char * alsValue;			//Alias Value String
	int used;			//This will be used to prevent alias circularization 

} als;

//Initialization for Alias Table
als alsTab[MAXALIAS];
als* curAls;
int numTabAls;



char cwd[1024];
char ** environment;
int environmentcount;
const char* path;
const char* home;
pid_t pids;

int pid;
int pipeStatus;

int* pipeFds[49];
int numPipes;
int pipePos;
int lastPipePos;