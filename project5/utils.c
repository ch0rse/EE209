#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include "utils.h"

#define PROMPT_LEN 2

static char prompt_str[] = "% ";

char *get_homedir(){

	char *homedir;

	if ((homedir = getenv("HOME")) == NULL) {

		struct passwd *p;
    	p = getpwuid(getuid());

    	if (!p) {
    		perror("getpwuid");
    		return NULL;
    	}
    	homedir = p->pw_dir;
	}

	return homedir;
}

int prompt() {

	if (write(1,prompt_str,PROMPT_LEN) != PROMPT_LEN) {
		perror("write");
		return 0;
	}
	return 1;
}

char *read_cmdline() {

	char buf[MAX_CMDLINE_LEN];

	if (!fgets(buf, sizeof(buf), stdin)) {
		perror("fgets");
		return NULL;
	}

	return strdup(buf);
}