#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include "dynarray.h"
#include "main.h"
#include "utils.h"
#include "dfa.h"

extern char **environ;
static time_t last_time = 0;
static DynArray_T plist = NULL;
char *prog_name;

struct process {
	pid_t pid;
	int pipefd[2];
};

void exec_nowait(DynArray_T tokens, int stdin_fd, int stdout_fd, int close_fd) {

	/* check if < or > is present in args */
	size_t i, len;
	enum RedirMode {REDIR_STDIN, REDIR_STDOUT, REDIR_NONE};
	enum RedirMode mode = REDIR_NONE;
	DynArray_T sliced = NULL;

	len = DynArray_getLength(tokens);

	for (i = 0; i < len; i++) {
		struct Token *cur = DynArray_get(tokens, i);
		if (cur->pcValue[0] == '<' && cur->eType == TOKEN_PIPE) {
			mode = REDIR_STDIN;
			break;
		}
		if (cur->pcValue[0] == '>' && cur->eType == TOKEN_PIPE) {
			mode = REDIR_STDOUT;
			break;
		}

		if (cur->pcValue[0] == '>' && cur->eType == TOKEN_PIPE) {
			assert(1==0);
		}
	}

	if (mode == REDIR_STDIN) {
		if (stdin_fd != -1) {
			LogErr("Multiple redirection of standard in/out");
			return;
		}
		struct Token *filename = DynArray_get(tokens, i+1);
		sliced = DynArray_slice(tokens,0,i);
		FILE *fp = fopen(filename->pcValue,"r");
		if (!fp) {
			LogErr("file does not exist");
			return;
		}
		stdin_fd = fileno(fp);
	}
	
	else if (mode == REDIR_STDOUT) {
		if (stdout_fd != -1) {
			LogErr("Multiple redirection of standard in/out");
			return;
		}

		struct Token *filename = DynArray_get(tokens, i+1);
		sliced = DynArray_slice(tokens,0,i);
		FILE *fp = fopen(filename->pcValue,"w");
		if (!fp) {
			LogErr("permission denied");
			return;
		} 
		stdout_fd = fileno(fp);
		if (fchmod(stdout_fd, S_IWUSR|S_IRUSR)) {
			LogErr("permission denied");
			return;
		}
	}

	if (!sliced) {
		sliced = DynArray_slice(tokens,0,len);
	}
	

	pid_t pid = fork();

	if (pid == -1) {
		perror("fork");
	}

	if (!pid) {
		if (stdin_fd != -1) {
			if (dup2(stdin_fd, 0)!=0) {
				perror("dup2");
				exit(0);
			}
		}
		if (stdout_fd != -1) {
			if (dup2(stdout_fd, 1)!=1) {
				perror("dup2");
				exit(0);
			}
		}
		if (close_fd != -1) {
			if (close(close_fd)) {
				perror("close");
				exit(0);
			}
		}
		/* remove all signal handlers */
		if (signal(SIGINT, NULL) == SIG_ERR) {
			perror("signal");
			exit(0);
		}
		if (signal(SIGQUIT, NULL) == SIG_ERR) {
			perror("signal");
			exit(0);
		}
		/* check for > or < symbol */
		char **args = make_argv(sliced);

		if (execvp(args[0], args)) {
			perror("execvp");
			exit(0);
		}

		free(args);

	} else {
	}	
}


void sigint_handler(int signo) {
	/* do nothing */
	return;
}

void sigquit_handler(int signo) {

	time_t current_time = time(NULL);
	
	if (current_time - last_time <= 5) {
		exit(0);
	} else {
		last_time = current_time;
		printf("Type Ctrl-\\ again within 5 seconds to exit.\n");
	}
}

void handle_setenv (char **argv) {
	if (argv[2] == NULL){
		if(setenv(argv[1],"",1) == -1){
			perror("setenv");
		}
	}
	else if(setenv(argv[1],argv[2],1) == -1) {
		perror("setenv");
	}
}
void handle_unsetenv (char **argv) {
	if(unsetenv(argv[1]) == -1) {
		perror("unsetenv");
	}
}
void handle_cd (char **argv) {
	if (!argv[1]) {
		/* cd to home directory */
		char *homedir = get_homedir();
		if (!homedir) {
			fputs("cd: could not locate home directory",stderr);
			return;
		}

		if (chdir(homedir) == -1) {
			perror("chdir");
			return;
		}
	}

	else if(chdir(argv[1]) == -1) {
		perror("chdir");
	}
}

void pexec_r(DynArray_T largs, DynArray_T rargs, int stdin_fd) {
	/* get the output of largs and write it to pipe*/
	int pipefd[2];
	int found = 0;
  	size_t i, rlen;

	if (pipe(pipefd)){
		perror("pipe");
		return;
	}

	exec_nowait(largs, stdin_fd, pipefd[1], pipefd[0]);
	rlen = DynArray_getLength(rargs);

	/* handle rargs next, check if there exists a bar (|) token in rargs */

	for (i = 0; i < rlen; i++) {
		struct Token *cur = DynArray_get(rargs, i);
		if (cur->pcValue[0] == '|' && cur->eType == TOKEN_PIPE) {
			found = 1;
			break;
		}
	}

	if (!found) {
		exec_nowait(rargs, pipefd[0], -1, pipefd[1]);
		close(pipefd[1]);
		close(pipefd[0]);
	} else {
		DynArray_T lll = DynArray_slice(rargs, 0, i);
		DynArray_T rrr =  DynArray_slice(rargs, i+1, rlen);
		close(pipefd[1]);
		pexec_r(lll, rrr, pipefd[0]);
		close(pipefd[0]);
		free_token_dynarr_preserve(lll);
		free_token_dynarr_preserve(rrr);
	}
	
}

void exec(DynArray_T tokens, int stdin_fd, int stdout_fd, int close_fd) {

	/* check if < or > is present in args */
	size_t i, len;
	enum RedirMode {REDIR_STDIN, REDIR_STDOUT, REDIR_NONE};
	enum RedirMode mode = REDIR_NONE;
	DynArray_T sliced = NULL;

	len = DynArray_getLength(tokens);

	for (i = 0; i < len; i++) {
		struct Token *cur = DynArray_get(tokens, i);
		if (cur->pcValue[0] == '<' && cur->eType == TOKEN_PIPE) {
			mode = REDIR_STDIN;
			break;
		}
		if (cur->pcValue[0] == '>' && cur->eType == TOKEN_PIPE) {
			mode = REDIR_STDOUT;
			break;
		}

		if (cur->pcValue[0] == '>' && cur->eType == TOKEN_PIPE) {
			assert(1==0);
		}
	}

	if (mode == REDIR_STDIN) {
		if (stdin_fd != -1) {
			LogErr("Multiple redirection of standard in/out");
			return;
		}
		struct Token *filename = DynArray_get(tokens, i+1);
		sliced = DynArray_slice(tokens,0,i);
		FILE *fp = fopen(filename->pcValue,"r");
		if (!fp) {
			LogErr("file does not exist");
			return;
		}
		stdin_fd = fileno(fp);
	}
	
	else if (mode == REDIR_STDOUT) {
		if (stdout_fd != -1) {
			LogErr("Multiple redirection of standard in/out");
			return;
		}

		struct Token *filename = DynArray_get(tokens, i+1);
		sliced = DynArray_slice(tokens,0,i);
		FILE *fp = fopen(filename->pcValue,"w");
		if (!fp) {
			LogErr("file does not exist");
			return;
		}
		stdout_fd = fileno(fp);
		if (fchmod(stdout_fd, S_IWUSR|S_IRUSR)) {
			LogErr("permission denied");
			return;
		}
	}

	if (!sliced) {
		sliced = DynArray_slice(tokens,0,len);
	}
	

	pid_t pid = fork();

	if (pid == -1) {
		perror("fork");
	}

	if (!pid) {
		if (stdin_fd != -1) {
			if (dup2(stdin_fd, 0)!=0) {
				perror("dup2");
				exit(0);
			}
		}
		if (stdout_fd != -1) {
			if (dup2(stdout_fd, 1)!=1) {
				perror("dup2");
				exit(0);
			}
		}
		if (close_fd != -1) {
			if (close(close_fd)) {
				perror("close");
				exit(0);
			}
		}
		/* remove all signal handlers */
		if (signal(SIGINT, NULL) == SIG_ERR) {
			perror("signal");
			exit(0);
		}
		if (signal(SIGQUIT, NULL) == SIG_ERR) {
			perror("signal");
			exit(0);
		}
		/* check for > or < symbol */
		char **args = make_argv(sliced);

		if (execvp(args[0], args)) {
			perror("execvp");
			exit(0);
		}

		free(args);

	} else {
		/* wait for child */
		if (waitpid(pid,NULL,0)==-1){
			perror("waitpid");
			return;
		}

	}	
}

void eval(char *cmdline) {
	DynArray_T tokens;
	size_t len, i;
	int pipe_exists = 0;

	int lex_success = tokenize(cmdline, &tokens);

	if (!lex_success || DynArray_getLength(tokens) == 0) {
		return;
	}

	struct Token *firstarg = (struct Token *)DynArray_get(tokens,0);
	len = DynArray_getLength(tokens);
	
	/* check for builtins */
	if (!strcmp(firstarg->pcValue,"setenv")) {
		if (len !=2 && len != 3) {
			LogErr("setenv takes 1 or 2 arguments");
			return;
		}
		char **args = make_argv(tokens);
		handle_setenv(args);
		free_token_dynarr_preserve(tokens);
		free(args);
		return;
	}

	if (!strcmp(firstarg->pcValue,"unsetenv")) {
		if (len != 2) {
			LogErr("unsetenv takes 1 argument");
			return;
		}
		char **args = make_argv(tokens);
		handle_unsetenv(args);
		free_token_dynarr_preserve(tokens);
		free(args);
		return;
	}

	if (!strcmp(firstarg->pcValue,"cd")) {
		if (len != 1 && len !=2) {
			LogErr("cd takes 1 or 2 arguments");
			return;
		}
		char **args = make_argv(tokens);
		handle_cd(args);
		free_token_dynarr_preserve(tokens);
		free(args);
		return;
	}

	if (!strcmp(firstarg->pcValue,"exit")) {
		if (len != 1) {
			LogErr("exit does not take an argument");
			return;
		}
		exit(0);
	}
	
	/* execute pexec_r only if there is pipe token */
	for (i = 0; i < len; i++) {
		struct Token *cur = DynArray_get(tokens,i);
		if (cur->pcValue[0] == '|' && cur->eType == TOKEN_PIPE ) {
			DynArray_T ltokens = DynArray_slice(tokens,0,i);
			DynArray_T rtokens = DynArray_slice(tokens,i+1,len);
			pexec_r(ltokens, rtokens,-1);
			pipe_exists = 1;
			while(waitpid(-1,NULL,0) != -1){	};
			/* free largs and rargs */
			break;
		}
	}

	if (!pipe_exists) {
		exec(tokens,-1,-1,-1);
	}
	
}

int ish_init() {

	char *homedir;
	FILE *fp;
	char line_buf[MAX_CMDLINE_LEN];
	/* init process vectors */
	plist = DynArray_new(0);
	if (!plist) {
		LogErr("plist failed to initialize");
		exit(0);
	}

	/* first install signal handlers they will be inherited to children so make sure to remove them before execve*/
	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
		perror("signal");
		return 0;
	}
	
	if (signal(SIGQUIT, sigquit_handler) == SIG_ERR) {
		perror("signal");
		return 0;
	}

	/* read .ishrc and eval each line */

	homedir = get_homedir();

	if (!homedir) {
		return 0;
	}

	size_t len = strlen(homedir) + strlen("/.ishrc") + 10;
	char *ishrc_path = malloc(len);
	memset(ishrc_path, 0, len);

	if (snprintf(ishrc_path, len-1, "%s/.ishrc", homedir) > len) {
		LogErr("error: home directory path is too long");
		return 0;
	}

	fp = fopen(ishrc_path, "r");

	if(!fp) {
		char buf[0x100];
		snprintf(buf,sizeof(buf),"ishrc at path %s was not found",ishrc_path);
		LogErr(buf);
		return 0;
	}

	while(fgets(line_buf, sizeof(line_buf), fp)) {
		char *ptr = strchr(line_buf,'\n');
		fflush(fp);
		/* remove \n at line_buf */
		if (ptr!=NULL) {
			*ptr = 0;
		}
		printf("%% %s\n",line_buf);
		eval(line_buf);
	}

	fclose(fp);

	return 1;
}


int main(int argc, char **argv) {

	/* register argv[0] */
	prog_name = strdup(argv[0]);
	if (!prog_name) {
		perror("strdup");
		LogErr("argv copying failed, exiting");
		exit(-1);
	}

	ish_init();

	while(1) {

		if (!prompt()) {
			LogErr("prompt failed");
			continue;
		}

		char *cmdline = read_cmdline();
		if (!cmdline) {
			free(cmdline);
			break;
		} else {
			eval(cmdline);
			free(cmdline);
		}


	}

	return 0;
}