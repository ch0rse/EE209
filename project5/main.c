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

void eval(char *cmdline) {
	char **tokens;
	size_t len, i;
	pid_t pid;

	int lex_success = tokenize(cmdline, &tokens, &len);

	if (!lex_success || len == 0) {
		return;
	}
	
	/* check for builtins */
	if (!strcmp(tokens[0],"setenv")) {
		if (len < 3) {
			fputs("setenv requires at least 2 arguments\n",stderr);
			return;
		}
		handle_setenv(tokens);
		return;
	}

	if (!strcmp(tokens[0],"unsetenv")) {
		if (len < 2) {
			fputs("unsetenv requires at least 1 argument\n",stderr);
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
	/* if not in builtin, exeute it */

	pid = fork();

	if (pid == -1) {
		perror("fork");
		for (i = 0; i < len-1; i++) {
			free(tokens[i]);
		}
		free(tokens);
		return;
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

		if (execvp(tokens[0], tokens) == -1) {
			perror("execve");
		}
	} else {
		/* wait for child */
		if (waitpid(pid, NULL, 0) < 0) {
			perror("waitpid");
		}

		/* free tokens vector and each component inside */
		for (i = 0; i < len-1; i++) {
			free(tokens[i]);
		}
		free(tokens);
	}	
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