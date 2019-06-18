/* handle command from commandline */
void handle_setenv (char **argv);
void handle_unsetenv (char **argv);
void handle_cd (char **argv);
void handle_command (char *command);
char *get_homedir();
/* pre-load command lines from .ishrc and install signal handlers */
int ish_init (void);
/* executes largs | rargs in a recursive manner*/
void pexec_r(char **largs, char **rargs);
/* executes args */
void exec(char **args);