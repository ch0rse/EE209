#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 

int main (int argc, char **argv, char **envp){
	int state = 0;
	int prev = 0;
	int alive = 1;

	unsigned long comment_entryline = 0;
	unsigned long numchars = 0;
	unsigned long numwords = 0;
	unsigned long numlines = 0;

	char c;
	int i = 0;

	/* reads characters until EOF */
	while((c = getchar()) && alive){
		/*add a line if a character is read from the initial state */
		//printf("state: %d, char: %c\n",state,c);
		switch (state) {

			case 0:
				numlines = 1;
				prev = 0;
				if (c == EOF) {
					alive = 0;
				}

				else if (isspace(c) && c!='\n'){
					state = 2;
				}

				else if (c == '/'){

					state = 4;
				}

				else if (c == '\n'){
					state = 3;
				}

				else {
					state = 1;
				}

			break;

			case 1:

			
				if (prev != 1 && prev != 4){
					//printf("reached 1 at %c, numwords=%lu (state = %d, iters = %d)\n",c,numwords,state,i);
					numwords++;
				}
				//printf("%d -> %d (state %d)\n",numchars,numchars+1,state);
				numchars++;
				prev = 1;

				if (c == EOF) {
					alive = 0;
				}

				else if (isspace(c) && c!='\n'){
					state = 2;
				}

				else if (c == '/'){
					state = 4;
				}

				else if (c == '\n'){
					state = 3;
				}

				else {
					state = 1;
				}

			break;

			case 2:
				//printf("%d -> %d (state %d)\n",numchars,numchars+1,state);
				numchars++;
				prev = 2;
				
				if (c == EOF) {
					alive = 0;
				}


				else if (isspace(c) && c!='\n'){
					state = 2;
				}

				else if (c == '/'){
					state = 4;
				}

				else if (c == '\n'){
					state = 3;
				}

				else {
					state = 1;
				}

			break;

			case 3:

				//printf("%d -> %d (state %d)\n",numchars,numchars+1,state);
				numchars++;
				numlines++;
				prev = 3;

				if (c == EOF) {
					alive = 0;
				}

				else if (isspace(c) && c!='\n'){
					state = 2;
				}

				else if (c == '/'){
					state = 4;
				}

				else if (c == '\n'){
					state = 3;
				}

				else {
					state = 1;
				}

			break;

			case 4:

				if (c != '*') {
					numchars++;
				}

				if ((prev == 0 || prev == 2 || prev == 3) && c != '*') {
					//printf("reached 2 at %c, numwords=%lu (state = %d, iters = %d)\n",c,numwords,state,i);
					numwords++;
				}

				prev = 4;

				if (c == EOF) {
					alive = 0;
				}

				else if (c == '*') {
					state = 5;
				}

				else if (isspace(c) && c!='\n'){
					state = 2;
				}

				else if (c == '/'){
					state = 4;
				}

				else if (c == '\n'){
					state = 3;
				}

				else {
					state = 1;
				}

			break;

			case 5:
				comment_entryline = numlines;

				if (prev != 4 && prev != 5 && prev !=6) {
					fprintf(stderr,"UNREACHABLE CODE!!!\n");
					exit(-1);
				}
				
				prev = 5;

				if (c == EOF) {
					alive = 0;
					fprintf(stderr,"Error: line %lu: unterminated comment\n",numlines);
					exit (EXIT_FAILURE);
				}

				else if (c == '*') {
					state = 6;
				}

				else {
					if (c == '\n') {
						numlines++;
						numchars++;
					}
					state = 5;
				}

			break;

			case 6:
				prev =6;

				if (c == EOF) {
					alive = 0;
					fprintf(stderr,"Error: line %lu: unterminated comment\n",comment_entryline);
					exit (EXIT_FAILURE);
				}

				else if (c == '/') {
					state = 2;
				}

				else if (c == '*'){
					state = 6;
				}
				else {
					if (c == '\n') {
						numchars++;
						numlines++;
					}
					state = 5;
				}
			break;

		}
		i++;
	}
	printf("%lu %lu %lu\n",numlines,numwords,numchars);
	exit (EXIT_SUCCESS);

	return 0;
}