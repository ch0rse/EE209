/* prevent memory leaks */
void free_token_dynarr(DynArray_T tokens);
/* tokenizes cmdline and makes argument array */
int tokenize (char *cmdline, char ***token_ptr, size_t *len_ptr);

/* lex err pipe: lex error with pipes */
/* lex err quote: quote that is not closed */
/* lex err nonalpha: use of non printable characters */
enum LexResult {LEX_SUCCESS, LEX_ERR_PIPE_MULTIPLE, LEX_ERR_PIPE_UNSPECIFIED, LEX_ERR_QUOTE, LEX_ERR_MEM, LEX_ERR_NPRINT};