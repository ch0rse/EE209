/*--------------------------------------------------------------------*/
/* dfa.c                                                              */
/* Original Author: Bob Dondero                                       */
/* Illustrate lexical analysis using a deterministic finite state     */
/* automaton (DFA)                                                    */
/*--------------------------------------------------------------------*/

#include "dynarray.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*--------------------------------------------------------------------*/

enum {MAX_LINE_SIZE = 1024};

enum {FALSE, TRUE};

enum TokenType {TOKEN_DQ, TOKEN_WORD, TOKEN_PIPE};

/*--------------------------------------------------------------------*/

/* A Token is either a number or a word, expressed as a string. */

struct Token
{
   enum TokenType eType;
   /* The type of the token. */

   char *pcValue;
   /* The string which is the token's value. */
};

/*--------------------------------------------------------------------*/

static void freeToken(void *pvItem, void *pvExtra)

/* Free token pvItem.  pvExtra is unused. */

{
   struct Token *psToken = (struct Token*)pvItem;
   free(psToken->pcValue);
   free(psToken);
}

static void freeToken_preserve_pcvalue(void *pvItem, void *pvExtra) {
   struct Token *psToken = (struct Token*)pvItem;
   free(psToken);
}


/*--------------------------------------------------------------------*/

static struct Token *makeToken(enum TokenType eTokenType,
   char *pcValue)

/* Create and return a Token whose type is eTokenType and whose
   value consists of string pcValue.  Return NULL if insufficient
   memory is available.  The caller owns the Token. */

{
   struct Token *psToken;

   psToken = (struct Token*)malloc(sizeof(struct Token));
   if (psToken == NULL)
      return NULL;

   psToken->eType = eTokenType;

   psToken->pcValue = (char*)malloc(strlen(pcValue) + 1);
   if (psToken->pcValue == NULL)
   {
      free(psToken);
      return NULL;
   }

   strcpy(psToken->pcValue, pcValue);

   return psToken;
}

/*--------------------------------------------------------------------*/

static int lexLine(const char *pcLine, DynArray_T oTokens)

/* Lexically analyze string pcLine.  Populate oTokens with the
   tokens that pcLine contains.  Return 1 (TRUE) if successful, or
   0 (FALSE) otherwise.  In the latter case, oTokens may contain
   tokens that were discovered before the error. The caller owns the
   tokens placed in oTokens. */

/* lexLine() uses a DFA approach.  It "reads" its characters from
   pcLine. */

{
   enum LexState {STATE_START, STATE_IN_DQ, STATE_IN_WORD};

   enum LexState eState = STATE_START;

   int iLineIndex = 0;
   int iValueIndex = 0;
   char c;
   char acValue[MAX_LINE_SIZE];
   struct Token *psToken;

   assert(pcLine != NULL);
   assert(oTokens != NULL);

   for (;;)
   {
      /* "Read" the next character from pcLine. */
      c = pcLine[iLineIndex++];
      switch (eState)
      {
         case STATE_START:
            if ((c == '\n') || (c == '\0'))
               return TRUE;
            
            else if (isspace(c))
               eState = STATE_START;

            else if (c == '"') {
               iValueIndex = 0;
               acValue[iValueIndex++] = c;
               eState = STATE_IN_DQ;
            }

            else if (c == '|') {
               acValue[0] = '|';
               acValue[1] = '\0';

               psToken = makeToken(TOKEN_PIPE, acValue);

               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               iValueIndex = 0;
               eState = STATE_START;
            }

            else if (c == '>') {
               acValue[0] = '>';
               acValue[1] = '\0';

               psToken = makeToken(TOKEN_PIPE, acValue);

               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               iValueIndex = 0;
               eState = STATE_START;
            }

            else if (c == '<') {
               acValue[0] = '<';
               acValue[1] = '\0';

               psToken = makeToken(TOKEN_PIPE, acValue);

               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               iValueIndex = 0;
               eState = STATE_START;
            }

            else if (isprint(c))
            {
               iValueIndex = 0;
               acValue[iValueIndex++] = c;
               eState = STATE_IN_WORD;
            }

            else
            {
               fprintf(stderr, "Invalid line at %c\n",c);
               return FALSE;
            }
            break;

         case STATE_IN_DQ:
            
            if (c == '"') {
               acValue[iValueIndex++] = c;
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_DQ, acValue);

               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               iValueIndex = 0;
               eState = STATE_START;
            }

            else if (isprint(c))
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_DQ;
            }

            else {
               fprintf(stderr, "Invalid line at %c\n",c);
               return FALSE;
            }
            break;

         case STATE_IN_WORD:
            if ((c == '\n') || (c == '\0'))
            {
               /* Create a WORD token. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               iValueIndex = 0;

               return TRUE;
            }
            
            else if (isspace(c))
            {
               /* Create a WORD token. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               iValueIndex = 0;

               eState = STATE_START;
            }

            else if (c == '|') {
               /* first, make token of current word */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               /* next add pipe token */
               acValue[0] = '|';
               acValue[1] = '\0';

               psToken = makeToken(TOKEN_PIPE, acValue);

               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               iValueIndex = 0;
               eState = STATE_START;
            }

            else if (c == '>') {
               /* first, make token of current word */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               /* next add pipe token */
               acValue[0] = '>';
               acValue[1] = '\0';

               psToken = makeToken(TOKEN_PIPE, acValue);

               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               iValueIndex = 0;
               eState = STATE_START;
            }

            else if (c == '<') {
               /* first, make token of current word */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               /* next add pipe token */
               acValue[0] = '<';
               acValue[1] = '\0';

               psToken = makeToken(TOKEN_PIPE, acValue);

               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               iValueIndex = 0;
               eState = STATE_START;
            }

            else if (isprint(c))
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_WORD;
            }

            else
            {
               fprintf(stderr, "Invalid line at %c\n",c);
               return FALSE;
            }
            break;

         default:
            assert(FALSE);
      }
   }
}

/*--------------------------------------------------------------------*/

void free_token_dynarr(DynArray_T tokens) {
   DynArray_map(tokens, freeToken, NULL);
   DynArray_free(tokens);
}

/* tokenize a command line passed from input */
int tokenize (char *cmdline, char ***token_ptr, size_t *len_ptr) {

   DynArray_T oTokens;
   int iSuccessful,i;
   
   oTokens = DynArray_new(0);
   if (oTokens == NULL)
   {
      fprintf(stderr, "Cannot allocate memory\n");
      exit(EXIT_FAILURE);
   }

   iSuccessful = lexLine(cmdline, oTokens);
   if (!iSuccessful) {
      /* free resources to prevent memory leak */
      free_token_dynarr(oTokens);
      return 0;
   } else {
      /* change dynarray to regular array */
      size_t len = DynArray_getLength(oTokens) + 1;

      struct Token **tokenv = calloc(sizeof(struct Token *), len);
      char **argv = calloc(sizeof(char *),len);

      DynArray_toArray(oTokens, (void **)tokenv);

      /* no need to null-terminate, due to calloc */

      for(i = 0; i < len-1; i++) {
         argv[i] = tokenv[i]->pcValue;
      }

      /* free resources */
      free(tokenv);

      /* you should not free each token's pcValue, this should be done by the caller*/
      DynArray_map(oTokens, freeToken_preserve_pcvalue, NULL);
      DynArray_free(oTokens);

      *len_ptr = len-1;
      *token_ptr = argv;
      return 1;
   }
}
