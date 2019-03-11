#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv, char **envp){
	int state = 0;
	unsigned long numchars = 0;
	unsigned long numwords = 0;
	unsigned long numlines = 0;

	char c;

	/* reads characters until EOF */
	while((c = getchar())!=EOF){
		/*add a line if a character is read from the initial state */
		if(!state){
			numlines++;
		}
		numchars++;
	}
	printf("%llu %llu %llu\n",numlines,numwords,numchars);

	return 0;
}