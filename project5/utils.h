/* each command line is restricted to 0x1000 bytes */
#define MAX_CMDLINE_LEN 0x1000

/* prints prompt, returns 1 on success and 0 on failure */
int prompt();
/* reads a line from stdin*/
char *getline();