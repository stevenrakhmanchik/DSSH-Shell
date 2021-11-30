void removeWhite(char *line); //Function that removes excess whitespace from end of command
void readLine(char *line); //Calls removeWhite function and then takes in stdin input
void parser(char *input_string, char **argv, int *wait); //parses input from command line
int checkRedir(char **argv); //checks if redirection takes place and returns true or false as 0 or 1
int checkPipe(char **argv);//similar to checkRedir but works with pipes
void parseRedir(char **argv, char **redirect_argv, int redirect_index); //parses command for redirection
void parsePipe(char **argv, char **child01_argv, char **child02_argv, int pipe_index); //parses command for pipe
void runChild(char **argv); //runs command as a child of the main process
void writeFromFile(char **argv, char **dir); //takes input from file for redirection
void writeToFile(char **argv, char **dir); //sends output to file from redirection
void appendToFile(char **argv, char **dir); //adds output to a file from redirection
void runPipe(char **argv_in, char **argv_out); //runs a command and uses its output for another command
void histAdd(char *history, char *line); //adds to history log
char *histGet(char *history); //gets history from log
int cdCommand(char **args); //built in function for changing current directory
int shellHelp(char **args); //displays help menu with info on built-in commands (Built-in commands do not support redirection)
int shellExit(char **args); //called by exit command: exits the shell
void runCommand(char **args, char **redir_argv, int wait, int res); //runs a command through the system dotfiles
int shellLs(char **args); //implements 'listdir' command
int shellRemove(char **args); //implements 'rmv' built-in command
int shellExRemove(char **args); //implements 'exrmv' built-in command
int shellHistory(char *history, char **redir_args); //keeps track of entire history of the commands run by the shell
int shellRedirect(char **args, char **redir_argv); //calls write and appoint functions after figuring out which redirection symbol was used
int shellPipe(char **args); //runs parsePipe and runPipe commands as helper functions to set up a pipe
void runCommand(char **args, char **redir_argv, int wait, int res); // runs a command
int main(void); //contains main shell loop that displays the prompt


#include <stdio.h>  
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h> 
#include <sys/stat.h>
#include <fcntl.h> 
#include <time.h>
#include <dirent.h>
#include <ftw.h>
#include <limits.h>
#include <errno.h>
#include<sys/utsname.h>
#include <stdbool.h>
#include <termios.h>

#define MAX_LINE_LENGTH 1024
#define BUFFER_SIZE 64
#define REDIR_SIZE 2
#define PIPE_SIZE 3
#define MAX_HISTORY_SIZE 128
#define MAX_COMMAND_NAME_LENGTH 128

#ifndef MAX_BUF
#define MAX_BUF 200
#endif

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define PROMPT_FORMAT "%F %T "
#define PROMPT_MAX_LENGTH 30

#ifdef _WIN32{
    printf("%sDSSH %s| %sYou are on windows. This will not work.", KCYN, KWHT, KRED);
		exit();
#endif

#define TOFILE_DIRECT ">"
#define APPEND_TOFILE_DIRECT ">>"
#define FROMFILE "<"
#define PIPE_OPT "|"