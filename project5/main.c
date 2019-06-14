#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "dynarray.h"
#include "main.h"
#include "utils.h"

void handle_command(char *cmdline) {
	printf("this is your command line: %s\n",cmdline);
}

int ish_init() {
	return 1;
}


int main(int argc, char **argv) {

	if (!ish_init()) {
		/* don't terminate the shell even if ish init fails */
		fputs("ish_init failed",stderr);
	}

	while(1) {

		if (!prompt()) {
			fputs("prompt failed",stderr);
			continue;
		}

		char *cmdline = getline();
		if (!cmdline) {
			fputs("getline failed",stderr);
		} else {
			handle_command(cmdline);
			free(cmdline);
		}


	}

	return 0;
}