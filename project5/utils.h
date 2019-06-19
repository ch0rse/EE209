/* each command line is restricted to 0x1000 bytes */
#define MAX_CMDLINE_LEN 0x1000
#define ISH_LOGERR(x) fprintf(stderr,x);

/* prints prompt, returns 1 on success and 0 on failure */
int prompt();
/* reads a line from stdin*/
char *read_cmdline();
/* for debug purposes, dump argv */
void dump_stack(char **args);
/* log error in the format of programname: error */
void LogErr(char *err_str);