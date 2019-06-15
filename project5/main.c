#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include "dynarray.h"
#include "main.h"
#include "utils.h"
#include "dfa.h"

extern char **environ;

void eval(char *cmdline) {
	char **tokens;
	size_t len, i;
	pid_t pid;

	int lex_success = tokenize(cmdline, &tokens, &len);

	if (!lex_success || len == 0) {
		return;
	}
	
	/* check for builtins */
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

	if ((homedir = getenv("HOME")) == NULL) {
		struct passwd *p;
    	p = getpwuid(getuid());

    	if (!p) {
    		perror("getpwuid");
    		return 0;
    	}

    	homedir = p->pw_dir;
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

		char *cmdline = getline();
		if (!cmdline) {
			fputs("getline failed\n",stderr);
		} else {
			eval(cmdline);
			free(cmdline);
		}


	}

	return 0;
}