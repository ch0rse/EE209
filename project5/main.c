#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <time.h>
#include "dynarray.h"
#include "main.h"
#include "utils.h"
#include "dfa.h"

extern char **environ;
static time_t last_time = 0;

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
	if(setenv(argv[1],argv[2],1) == -1) {
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
			fputs("cd: could not locate home directory\n",stderr);
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

/* assume the last token is NULL */
void free_tokens(char **tokens, size_t len) {
	size_t i;
	for (i = 0;i < len;i++) {
		if (tokens[i]) {
			free(tokens[i]);
		}
		
	}
	free(tokens);
}

void pexec_r(char **largs, char **rargs, int readpipe) {
	/* get the output of largs and write it to pipe*/
	int pipefd[2];
	int pipe_exists = 0;
	pid_t pid;
	size_t i;


	if (pipe(pipefd)){
		perror("pipe");
		return;
	}

	pid = fork();
	if (pid == -1) {
		perror("fork");
		return;
	}

	if (!pid) {
		/* child process, dup2, close reading pipe and execute */
		if (readpipe != -1) {
			if (dup2(readpipe,0) != 0) {
				perror("dup2");
				return;
			}
		}
		if (dup2(pipefd[1],1) != 1){
			perror("dup2");
			return;
		}

		if (close(pipefd[0])) {
			perror("close");
			return;
		}

		/* remove all signal handlers */
		if (signal(SIGINT, NULL) == SIG_ERR) {
			perror("signal");
			return;
		}

		if (signal(SIGQUIT, NULL) == SIG_ERR) {
			perror("signal");
			return;
		}

		if (execvp(largs[0], largs)) {
			perror("execvp");
		}

		
	} else {
		/* parent process */
		if (close(pipefd[1])) {
			perror("close");
			return;
		}
		/* check if pipe token exists in rargs */
		for (i = 0;;i++){
			if (!rargs[i]) {
				break;
			}
			if (rargs[i][0] == '|') {
				rargs[i] = NULL;
				pipe_exists = 1;
				break;
			}
		}

		/* if pipe token does not exist, execute rargs */
		if (!pipe_exists) {
			pid_t pid = fork();

			if (pid == -1) {
				perror("fork");
			}

			if (!pid) {

				/* remove all signal handlers */
				if (signal(SIGINT, NULL) == SIG_ERR) {
					perror("signal");
					return;
				}

				if (signal(SIGQUIT, NULL) == SIG_ERR) {
					perror("signal");
					return;
				}

				if (dup2(pipefd[0],0) != 0) {
					perror("dup2");
					return;
				}

				if (execvp(rargs[0], rargs)) {
					perror("execvp");
					return;
				}
			} else {
				/* wait for child */
				if (waitpid(pid, NULL, 0) < 0) {
					perror("waitpid");
				}
			}	
		}
		/* if pipe does exist, call pexec_r in a recursive manner */
		else {
			/* must connect with pipefd[0]<-stdin */
			pexec_r(&rargs[0],&rargs[i+1], pipefd[0]);	
		}
	
		/* wait for child to end */
		if (waitpid(pid, NULL, 0) < 0) {
			perror("waitpid");
		}
		
	}

}

void exec(char **args) {
	pid_t pid = fork();

	if (pid == -1) {
		perror("fork");
	}

	if (!pid) {

		/* remove all signal handlers */
		if (signal(SIGINT, NULL) == SIG_ERR) {
			perror("signal");
			return;
		}

		if (signal(SIGQUIT, NULL) == SIG_ERR) {
			perror("signal");
			return;
		}

		if (execvp(args[0], args)) {
			perror("execvp");
		}
	} else {
		/* wait for child */
		if (waitpid(pid, NULL, 0) < 0) {
			perror("waitpid");
		}
	}	
}

void eval(char *cmdline) {
	char **tokens;
	size_t len, i;
	int pipe_exists = 0;

	int lex_success = tokenize(cmdline, &tokens, &len);

	if (!lex_success || len == 0) {
		return;
	}
	
	/* check for builtins */
	if (!strcmp(tokens[0],"setenv")) {
		if (len !=2 || len != 3) {
			fputs("setenv requires 1 or 2 arguments\n",stderr);
			return;
		}
		handle_setenv(tokens);
		return;
	}

	if (!strcmp(tokens[0],"unsetenv")) {
		if (len != 2) {
			fputs("unsetenv takes 1 argument\n",stderr);
			return;
		}
		handle_unsetenv(tokens);
		return;
	}

	if (!strcmp(tokens[0],"cd")) {
		handle_cd(tokens);
		return;
	}

	if (!strcmp(tokens[0],"exit")) {
		exit(0);
	}
	
	/* execute pexec_r only if there is pipe token */
	for (i = 0; i < len; i++) {
		if (!tokens[i]) {
			break;
		}
		if (tokens[i][0] == '|') {
			tokens[i] = NULL;
			pexec_r(&tokens[0], &tokens[i+1],-1);
			pipe_exists = 1;
			break;
		}
	}

	if (!pipe_exists) {
		exec(tokens);
	}
	
	/* free tokens */
	free_tokens(tokens, len);

	
}

int ish_init() {

	char *homedir;
	FILE *fp;
	char line_buf[MAX_CMDLINE_LEN];

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
		fputs("error: home directory path is too long",stderr);
		return 0;
	}

	fp = fopen(ishrc_path, "r");

	if(!fp) {
		fprintf(stderr,"ishrc at path %s was not found\n",ishrc_path);
		return 0;
	}

	while(fgets(line_buf, sizeof(line_buf), fp)) {
		printf("%% %s\n",line_buf);
		eval(line_buf);
	}

	return 1;
}


int main(int argc, char **argv) {

	if (!ish_init()) {
		/* don't terminate the shell even if ish init fails */
		fputs("ish_init failed\n",stderr);
	}

	while(1) {

		if (!prompt()) {
			fputs("prompt failed\n",stderr);
			continue;
		}

		char *cmdline = read_cmdline();
		if (!cmdline) {
			fputs("getline failed\n",stderr);
		} else {
			eval(cmdline);
			free(cmdline);
		}


	}

	return 0;
}