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

int running = 1;

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	int rv = remove(fpath);

	if (rv)
		perror(fpath);

	return rv;
}

int unlink_cb_verb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	int rv = remove(fpath);
	if (rv)
		perror(fpath);
	else	printf("%s\n", fpath);
	return rv;
 }

void runShell() {
  system("clear");
    struct utsname uts;
    uname(&uts);
    printf("%s%s %s %son %s%s\n", KCYN, uts.sysname, uts.release, KWHT, KCYN, uts.machine);
    printf("%sDSSH Shell %sBy %sSteven Rakhmanchik %sand %sDerrick Lin\n\n", KCYN, KWHT, KCYN, KWHT, KCYN);
}

char *get_current_dir() {
    char cwd[FILENAME_MAX];
    char*result = getcwd(cwd, sizeof(cwd));
    return result;
}

char *prompt() {
    static char *_prompt = NULL;
    time_t now;
    struct tm *tmp;
    size_t size;

    if (_prompt == NULL) {
        _prompt = malloc(PROMPT_MAX_LENGTH * sizeof(char));
        if (_prompt == NULL) {
            printf("%sDSSH %s| %sMemory allocation error\n", KCYN, KWHT, KRED);
            exit(EXIT_FAILURE);
        }
    }
    char* username = getenv("USER");
    strncat(_prompt, username, strlen(username));
    return _prompt;
}

void removeWhite(char *line) {
    int i = 0;
    while (line[i] != '\n') {
        i++;
    }

    line[i] = '\0';
}

void readLine(char *line) {
    char *ret = fgets(line, MAX_LINE_LENGTH, stdin);

    
    removeWhite(line);

    
    if (strcmp(line, "exit") == 0 || ret == NULL || strcmp(line, "quit") == 0) {
        exit(EXIT_SUCCESS);
    }
}

void parser(char *input_string, char **argv, int *wait) {
    int i = 0;

    while (i < BUFFER_SIZE) {
        argv[i] = NULL;
        i++;
    }

    
    *wait = (input_string[strlen(input_string) - 1] == '&') ? 0 : 1; 
    input_string[strlen(input_string) - 1] = (*wait == 0) ? input_string[strlen(input_string) - 1] = '\0' : input_string[strlen(input_string) - 1];
    i = 0;
    argv[i] = strtok(input_string, " ");

    if (argv[i] == NULL) return;

    while (argv[i] != NULL) {
        i++;
        argv[i] = strtok(NULL, " ");
    }

    argv[i] = NULL;
}

int checkRedir(char **argv) {
    int i = 0;
    while (argv[i] != NULL) {
        if (strcmp(argv[i], TOFILE_DIRECT) == 0 || strcmp(argv[i], APPEND_TOFILE_DIRECT) == 0 || strcmp(argv[i], FROMFILE) == 0) {
            return i; 
        }
        i = -~i; 
    }
    return 0; 
}

int checkPipe(char **argv) {
    int i = 0;
    while (argv[i] != NULL) {
        if (strcmp(argv[i], PIPE_OPT) == 0) {
            return i; 
        }
        i = -~i; 
    }
    return 0; 
}

void parseRedir(char **argv, char **redirect_argv, int redirect_index) {
    redirect_argv[0] = strdup(argv[redirect_index]);
    redirect_argv[1] = strdup(argv[redirect_index + 1]);
    argv[redirect_index] = NULL;
    argv[redirect_index + 1] = NULL;
}

void parsePipe(char **argv, char **child01_argv, char **child02_argv, int pipe_index) {
    int i = 0;
    for (i = 0; i < pipe_index; i++) {
        child01_argv[i] = strdup(argv[i]);
    }
    child01_argv[i++] = NULL;

    while (argv[i] != NULL) {
        child02_argv[i - pipe_index - 1] = strdup(argv[i]);
        i++;
    }
    child02_argv[i - pipe_index - 1] = NULL;
}

void runChild(char **argv) {
    if (execvp(argv[0], argv) < 0) {
        printf("%sDSSH %s| %sError while executing command\n", KCYN, KWHT, KRED);
        exit(EXIT_FAILURE);
    }
}

void writeFromFile(char **argv, char **dir) {
    
    int fd_in = open(dir[1], O_RDONLY);
    if (fd_in == -1) {
        printf("%sDSSH %s| %sError while redirecting input\n", KCYN, KWHT, KRED);
        exit(EXIT_FAILURE);
    }

    dup2(fd_in, STDIN_FILENO);

    if (close(fd_in) == -1) {
        printf("%sDSSH %s| %sError while closing input\n", KCYN, KWHT, KRED);
        exit(EXIT_FAILURE);
    }
    runChild(argv);
}

void writeToFile(char **argv, char **dir) {
    

    int fd_out;
    fd_out = creat(dir[1], S_IRWXU);
    if (fd_out == -1) {
        printf("%sDSSH %s| %sError while redirecting output\n", KCYN, KWHT, KRED);
        exit(EXIT_FAILURE);
    }
    dup2(fd_out, STDOUT_FILENO);
    if (close(fd_out) == -1) {
        printf("%sDSSH %s| %sError while closing output\n", KCYN, KWHT, KRED);
        exit(EXIT_FAILURE);
    }

    runChild(argv);
}

void appendToFile(char **argv, char **dir) {
    
    int fd_out;
    if (access(dir[0], F_OK) != -1) {
        fd_out = open(dir[0], O_WRONLY | O_APPEND);
    }
    if (fd_out == -1) {
        printf("%sDSSH %s| %sError while redirecting output\n", KCYN, KWHT, KRED);
        exit(EXIT_FAILURE);
    }
    dup2(fd_out, STDOUT_FILENO);
    if (close(fd_out) == -1) {
        printf("%sDSSH %s| %sError while closing output\n", KCYN, KWHT, KRED);
        exit(EXIT_FAILURE);
    }
    runChild(argv);
}

void runPipe(char **argv_in, char **argv_out) {
    int fd[2];
    
    if (pipe(fd) == -1) {
        printf("%sDSSH %s| %sError while piping\n", KCYN, KWHT, KRED);
        exit(EXIT_FAILURE);
    }

    if (fork() == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        runChild(argv_in);
        exit(EXIT_SUCCESS);
    }  
    if (fork() == 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        close(fd[0]);
        runChild(argv_out);
        exit(EXIT_SUCCESS);
    }

    close(fd[0]);
    close(fd[1]);
    wait(0);
    wait(0);
}

void runParent(pid_t child_pid, int *bg) {}

void histAdd(char *history, char *line) {
    strcpy(history, line);
}

char *histGet(char *history) {
    if (history[0] == '\0') {
        printf("%sDSSH %s| %sNo Commands in history\n", KCYN, KWHT, KRED);
        return NULL;
    }
    return history;
}

int cdCommand(char **args);
int shellHelp(char **args);
int shellExit(char **args);
void runCommand(char **args, char **redir_argv, int wait, int res);


char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "listdir",
    "rmv",
    "exrmv"};

int shellLs(char **args) {
	struct dirent **namelist;
	int n = scandir(".", &namelist, NULL, alphasort);
	if(n < 0)
	{
		perror("sh");
	}
	else
	{
		int i;
		i = 0;
		while (n--)
		{
			i++;
			printf("%s\n", namelist[n]->d_name);
			free(namelist[n]);
		}
		free(namelist);
	}
	return 1;
}

int shellRemove(char **args) {
	int argc = 1;
	bool RECURSIVE_FLAG = false,
		VERBOSE_FLAG = false,
		FORCE_FLAG = false;
	char filename[2048] = "";

	while(args[argc] != NULL)
	{
		if (strcmp(args[argc], "-r") == 0) {
			RECURSIVE_FLAG = true;
		} else if (strcmp(args[argc], "-f") == 0) {
			FORCE_FLAG = true;
		} else if (strcmp(args[argc], "-v") == 0) {
			VERBOSE_FLAG = true;
		} else {
			if (args[argc][0] != '/') {
				getcwd(filename, sizeof(filename));
				strcat(filename, "/");
			}
			strcat(filename, args[argc]);
		}
		argc++;
	}

	if (filename == NULL) {
		printf("%sDSSH %s| %sExpected a file name for rm", KCYN, KWHT, KRED);
	} else if (!RECURSIVE_FLAG) {
		if (unlink(filename) != 0) {
			perror("DSSH");
			return 1;
		}
		if (VERBOSE_FLAG)	printf("%s\n", filename);
	} else {
		if(VERBOSE_FLAG)	nftw(filename, unlink_cb_verb, 64, FTW_DEPTH | FTW_PHYS);
		else	nftw(filename, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
	}
	return 1;
}

int shellExRemove(char **args) {
	int j, i = 0, flag = 0;

	while(args[i] != NULL)	i++;

	DIR *d;
	struct dirent *dir;

	d = opendir(".");
	if (d) {
		while ((dir = readdir(d)) != NULL) {

			if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
				continue;
			}

			for(j = 1; j < i; j++) {
				if( strcmp(args[j], dir->d_name) == 0 ) {
					flag = 0;
					break;
				} else {
					flag = 1;
				}
			}

			if (flag == 1) {
				char filename[2048] = "";
				getcwd(filename, sizeof(filename));
				strcat(filename, "/");
				strcat(filename, dir->d_name);
				if (unlink(filename) != 0) {
					perror("DSSH");
				}
				printf("%s\n", filename);
			}
		}

		closedir(d);
	}
	return 1;
}

int (*builtin_func[])(char **) = {
    &cdCommand,
    &shellHelp,
    &shellExit,
    &shellLs,
    &shellRemove,
    &shellExRemove
};

int builtIn() {
    return sizeof(builtin_str) / sizeof(char *);
}

int cdCommand(char **argv) {
    if (argv[1] == NULL) {
        printf("%sDSSH %s| %sArgument for command cd expected\n", KCYN, KWHT, KRED);
    } else {
        
        if (chdir(argv[1]) != 0) {
            printf("%sDSSH %s| %sError while changing directory to PATH\n", KCYN, KWHT, KRED);
        }
    }
    return 1;
}

int shellHelp(char **argv) {
    if (strcmp(argv[0], "help") == 0 && argv[1] == NULL) {
        printf("DSHH Help Information:\n");
	    printf("help        <- Displays help menu with all built-in commands\n");
	    printf("cd          <- Changes directory\n");
	    printf("listdir     <- Prints files in current directory\n");
	    printf("rmv         <- Removes directory, or file\n");
	    printf("exrmv       <- Removes all files except those specified\n");
	    printf("exit        <- Exits DSSH shell\n");
        return 0;
    }
    return 0;
}

int shellExit(char **args) {
    running = 0;
    return running;
}
int shellHistory(char *history, char **redir_args) {
    char *cur_args[BUFFER_SIZE];
    char cur_command[MAX_LINE_LENGTH];
    int t_wait;

    if (history[0] == '\0') {
        printf("%sDSSH %s| %sNo commands in history\n", KCYN, KWHT, KRED);
        return 1;
    }
    strcpy(cur_command, history);
    printf("%s\n", cur_command);
    parser(cur_command, cur_args, &t_wait);
    int res = 0;
    runCommand(cur_args, redir_args, t_wait, res);
    return res;
}
int shellRedirect(char **args, char **redir_argv) {
    
    int redir_op_index = checkRedir(args);
    
    if (redir_op_index != 0) {
        parseRedir(args, redir_argv, redir_op_index);
        if (strcmp(redir_argv[0], ">") == 0) {
            writeToFile(args, redir_argv);
        } else if (strcmp(redir_argv[0], "<") == 0) {
            writeFromFile(args, redir_argv);
        } else if (strcmp(redir_argv[0], ">>") == 0) {
            appendToFile(args, redir_argv);
        }
        return 1;
    }
    return 0;
}
int shellPipe(char **args) {
    int pipe_op_index = checkPipe(args);
    if (pipe_op_index != 0) {
        
        char *child01_arg[PIPE_SIZE];
        char *child02_arg[PIPE_SIZE];
        parsePipe(args, child01_arg, child02_arg, pipe_op_index);
        runPipe(child01_arg, child02_arg);
        return 1;
    }
    return 0;
}
void runCommand(char **args, char **redir_argv, int wait, int res) {
    
    for (int i = 0; i < builtIn(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            (*builtin_func[i])(args);
            res = 1;
        }
    }
    if (res == 0) {
        int status; 
        pid_t pid = fork();
        if (pid == 0) { 
            if (res == 0) res = shellRedirect(args, redir_argv);
            if (res == 0) res = shellPipe(args);
            if (res == 0) execvp(args[0], args);
            exit(EXIT_SUCCESS);
        } else if (pid < 0) { 
            printf("%sDSSH %s| %sEcountered error while forking\n", KCYN, KWHT, KRED);
            exit(EXIT_FAILURE);
        } else { 
            if (wait == 1) {
                waitpid(pid, &status, WUNTRACED); 
            }
        }
    }
}

int main(void) {
    char *args[BUFFER_SIZE];
    char line[MAX_LINE_LENGTH];
    char t_line[MAX_LINE_LENGTH];
    char history[MAX_LINE_LENGTH] = "No commands in history";
    char *redir_argv[REDIR_SIZE];
    int wait;
    runShell();
    int res = 0;
    while (running) {
        int getlogin_r(char *buf, size_t bufsize);
        char path[MAX_BUF];
        getcwd(path, MAX_BUF);
        char hostname[1024];
        char username[1024];
        gethostname(hostname, 1024);
        getlogin_r(username, 1024);
        printf("%s%s@%s %s%s %s$ ", KGRN, username, hostname, KCYN, path, KWHT);
        fflush(stdout);
        readLine(line);
        strcpy(t_line, line);
        parser(line, args, &wait);
        if (strcmp(args[0], "!!") == 0) {
            res = shellHistory(history, redir_argv);
        } else {
            histAdd(history, t_line);
            runCommand(args, redir_argv, wait, res);
        }
        res = 0;
    }
    return 0;
}
