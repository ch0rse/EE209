enum {MAX_LINE_SIZE = 1024};
enum {FALSE, TRUE};
enum TokenType {TOKEN_DQ, TOKEN_WORD, TOKEN_PIPE};

struct Token
{
   enum TokenType eType;
   /* The type of the token. */

   char *pcValue;
   /* The string which is the token's value. */
};

/* prevent memory leaks */
void free_token_dynarr(DynArray_T tokens);
/* tokenizes cmdline and makes argument array */
int tokenize (char *cmdline, DynArray_T *token_ptr);

/* lex err pipe: lex error with pipes */
/* lex err quote: quote that is not closed */
/* lex err nonalpha: use of non printable characters */
enum LexResult {LEX_SUCCESS, LEX_ERR_PIPE_MULTIPLE, LEX_ERR_PIPE_UNSPECIFIED, LEX_ERR_QUOTE, LEX_ERR_MEM, LEX_ERR_NPRINT};