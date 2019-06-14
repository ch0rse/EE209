#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"

#define PROMPT_LEN 2

static char prompt_str[] = "% ";

/* my implementation of strdup, using malloc */

static char *strdup(const char *ori) {
	size_t len = strlen(ori)+1;
	char *out = malloc(len);

	if (!out) {
		perror("malloc");
		return NULL;
	}

	strncpy(out, ori, len);
	return out;
}

int prompt() {

	if (write(1,prompt_str,PROMPT_LEN) != PROMPT_LEN) {
		perror("write");
		return 0;
	}
	return 1;
}

char *getline() {

	char buf[MAX_CMDLINE_LEN];

	if (!fgets(buf, sizeof(buf), stdin)) {
		perror("fgets");
		return NULL;
	}

	return strdup(buf);
}