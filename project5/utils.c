#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include "utils.h"

#define PROMPT_LEN 2

extern char *prog_name;
static char prompt_str[] = "% ";

void LogErr(char *errstr) {
	fprintf(stderr,"%s: %s\n",prog_name, errstr);
}

void dump_stack(char **args) {
	size_t i;
	for (i = 0;;i++) {
		if(!args[i]){
			break;
		}
		printf("%02lx: %s\n",i,args[i]);
	}
}

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
		return NULL;
	}

	return strdup(buf);
}
