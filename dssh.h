int breakCommand(char *str); // Contains if statements to identify which command should be run. Passes errors if command fails for whatever reason.
int breakString(char *str); // Acts as an initial parser identifying whether ; is used and passes some values to breakCommand to run certain built in commands such as exit.
int showPrompt(void); // Displays command prompt text and funnels it into next function
void runShell(); // Displays login statement with info on system and Kernel.
int main(int argc, char *argv[]); // Runs main portion of code and contains loop where the shell runs

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include<sys/utsname.h>

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

#ifdef _WIN32{
    printf("%sDSSH %s| %sYou are on windows. This will not work.", KCYN, KWHT, KRED);
		exit();
#endif
