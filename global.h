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

#define CDHome 1
#define CDPath 2
#define Start 3
#define Stop 4


void init();
void prompt();

int getCommand();
void processCommand();
void do_it();
void execute_it();
void handle_errors();
int understand_errors();
void init_Scanner_Parser();

char* get_curr_dir();
void changeDirectory(int);

int cd;
char* word;
char cwd[1024];
const char* home;
int builtin;
int isBuiltin;