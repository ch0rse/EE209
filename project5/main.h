/* handle command from commandline */
void handle_setenv (char **argv);
void handle_unsetenv (char **argv);
void handle_cd (char **argv);
void handle_command (char *command);
char *get_homedir();
/* pre-load command lines from .ishrc and install signal handlers */
int ish_init (void);
/* executes largs | rargs in a recursive manner*/
void pexec_r(DynArray_T largs, DynArray_T rargs, int stdin_fd);
/* executes args */
void exec(DynArray_T tokens, int stdin_fd, int stdout_fd, int close_fd);