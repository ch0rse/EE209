/*--------------------------------------------------------------------*/
/* dfa.c                                                              */
/* Original Author: Bob Dondero                                       */
/* Illustrate lexical analysis using a deterministic finite state     */
/* automaton (DFA)                                                    */
/*--------------------------------------------------------------------*/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dynarray.h"
#include "dfa.h"
#include "utils.h"


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

static int lexChk(DynArray_T tokens) {
   /* 1. separate all chunks of tokens with <, > and | 
      2. if filename consists of multiple tokens, return LEX_ERR_PIPE_FNAME_INVALID
      3. if a token is a filename and cmd at the same time, return LEX_ERR_PIPE_MULTIPLE
   */
   size_t i, len;
   int is_next_file = 0;

   len = DynArray_getLength(tokens);

   for (i = 0; i < len; i++) {
      struct Token *cur = DynArray_get(tokens,i);
      if (cur->pcValue[0] == '>' && cur->eType == TOKEN_PIPE) {
         is_next_file = 1;
         continue;
      }
      if (cur->pcValue[0] == '<' && cur->eType == TOKEN_PIPE) {
         is_next_file = 1;
         continue;
      }
      if (cur->pcValue[0] == '|' && cur->eType == TOKEN_PIPE) {
         is_next_file = 0;
         continue;
      }
      if (is_next_file) {
         if (i < len-1) {
            struct Token *next = DynArray_get(tokens,i+1);
            if (next->eType != TOKEN_PIPE) {
               return LEX_ERR_PIPE_FNAME_INVALID;
            }
            else if (next->pcValue[0] == '<') {
               return LEX_ERR_PIPE_MULTIPLE;
            }
            else if (next->pcValue[0] == '>') {
               return LEX_ERR_PIPE_MULTIPLE;
            }
         }
      }

   }
   return LEX_SUCCESS;
}

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
   int nbars, nstdinr, nstdoutr;

   nbars = 0;
   nstdinr = 0;
   nstdoutr = 0;

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
               return LEX_SUCCESS;
            
            else if (isspace(c))
               eState = STATE_START;

            else if (c == '"') {
               iValueIndex = 0;
               acValue[iValueIndex++] = c;
               eState = STATE_IN_DQ;
            }

            else if (c == '|') {

               /* make sure previous token is word or dq token */
               size_t len = DynArray_getLength(oTokens);
               if (len == 0){
                  return LEX_ERR_PIPE_UNSPECIFIED;
               }

               if (nstdoutr) {
                  return LEX_ERR_PIPE_MULTIPLE;
               }

               psToken = DynArray_get(oTokens,len-1);
               if (psToken->eType != TOKEN_DQ && psToken->eType != TOKEN_WORD) {
                  return LEX_ERR_PIPE_UNSPECIFIED;
               }

               acValue[0] = '|';
               acValue[1] = '\0';

               psToken = makeToken(TOKEN_PIPE, acValue);

               if (psToken == NULL)
               {
                  return LEX_ERR_MEM;
               }

               if (! DynArray_add(oTokens, psToken))
               {
                  return LEX_ERR_MEM;
               }

               iValueIndex = 0;
               eState = STATE_START;
               nbars++;
            }

            /* should be erroneous if before a | token */

            else if (c == '>') {

               /* make sure previous token is word or dq token */
               size_t len = DynArray_getLength(oTokens);
               if (len == 0){
                  return LEX_ERR_PIPE_UNSPECIFIED;
               }
               psToken = DynArray_get(oTokens,len-1);
               if (psToken->eType != TOKEN_DQ && psToken->eType != TOKEN_WORD) {
                  return LEX_ERR_PIPE_UNSPECIFIED;
               }

               acValue[0] = '>';
               acValue[1] = '\0';

               psToken = makeToken(TOKEN_PIPE, acValue);

               if (psToken == NULL)
               {
                  return LEX_ERR_MEM;
               }

               if (! DynArray_add(oTokens, psToken))
               {
                  return LEX_ERR_MEM;
               }

               iValueIndex = 0;
               eState = STATE_START;
               nstdoutr++;
            }

            /* should be erroneous if after a | token */

            else if (c == '<') {

               /* make sure previous token is word or dq token */
               size_t len = DynArray_getLength(oTokens);
               if (len == 0){
                  return LEX_ERR_PIPE_UNSPECIFIED;
               }
               psToken = DynArray_get(oTokens,len-1);
               if (psToken->eType != TOKEN_DQ && psToken->eType != TOKEN_WORD) {
                  return LEX_ERR_PIPE_UNSPECIFIED;
               }

               if (nbars) {
                  return LEX_ERR_PIPE_MULTIPLE;
               }

               acValue[0] = '<';
               acValue[1] = '\0';

               psToken = makeToken(TOKEN_PIPE, acValue);

               if (psToken == NULL)
               {
                  return LEX_ERR_MEM;
               }

               if (! DynArray_add(oTokens, psToken))
               {
                  return LEX_ERR_MEM;
               }

               iValueIndex = 0;
               eState = STATE_START;
               nstdinr++;
            }

            else if (isprint(c))
            {
               iValueIndex = 0;
               acValue[iValueIndex++] = c;
               eState = STATE_IN_WORD;
            }

            else
            {
               return LEX_ERR_NPRINT;
            }
            break;

         case STATE_IN_DQ:
            
            if (c == '"') {
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_DQ, &acValue[1]);

               if (psToken == NULL)
               {
                  return LEX_ERR_MEM;
               }

               if (! DynArray_add(oTokens, psToken))
               {
                  return LEX_ERR_MEM;
               }

               iValueIndex = 0;
               eState = STATE_START;
            }

            else if (isprint(c))
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_DQ;
            }

            /* termination without closing */
            else if ((c == '\n') || (c == '\0'))
            {
               return LEX_ERR_QUOTE;
            }

            else {
               return LEX_ERR_NPRINT;
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
                  return LEX_ERR_MEM;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return LEX_ERR_MEM;
               }
               iValueIndex = 0;

               return LEX_SUCCESS;
            }
            
            else if (isspace(c))
            {
               /* Create a WORD token. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  return LEX_ERR_MEM;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  return LEX_ERR_MEM;
               }
               iValueIndex = 0;

               eState = STATE_START;
            }

            else if (c == '|') {
               /* make sure previous token is word or dq token */
               size_t len = DynArray_getLength(oTokens);
               psToken = DynArray_get(oTokens,len-1);
               if (len == 0 || nstdoutr){
                  return LEX_ERR_PIPE_UNSPECIFIED;
               }
               if (psToken->eType != TOKEN_DQ && psToken->eType != TOKEN_WORD) {
                  return LEX_ERR_PIPE_UNSPECIFIED;
               }

               /* first, make token of current word */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  return LEX_ERR_MEM;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  return LEX_ERR_MEM;
               }
               /* next add pipe token */
               acValue[0] = '|';
               acValue[1] = '\0';

               psToken = makeToken(TOKEN_PIPE, acValue);

               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return LEX_ERR_MEM;
               }

               if (! DynArray_add(oTokens, psToken))
               {
                  return LEX_ERR_MEM;
               }

               iValueIndex = 0;
               eState = STATE_START;
               nbars++;
            }

            /* should be erroneous if before a | token */

            else if (c == '>') {
               /* make sure previous token is word or dq token */
               size_t len = DynArray_getLength(oTokens);
               if (len == 0){
                  return LEX_ERR_PIPE_UNSPECIFIED;
               }
               psToken = DynArray_get(oTokens,len-1);
               if (psToken->eType != TOKEN_DQ && psToken->eType != TOKEN_WORD) {
                  return LEX_ERR_PIPE_UNSPECIFIED;
               }

               /* first, make token of current word */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  return LEX_ERR_MEM;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  return LEX_ERR_MEM;
               }
               /* next add pipe token */
               acValue[0] = '>';
               acValue[1] = '\0';

               psToken = makeToken(TOKEN_PIPE, acValue);

               if (psToken == NULL)
               {
                  return LEX_ERR_MEM;
               }

               if (! DynArray_add(oTokens, psToken))
               {
                  return LEX_ERR_MEM;
               }

               iValueIndex = 0;
               eState = STATE_START;
               nstdoutr++;
            }

            /* should be erroneous if after a | token */

            else if (c == '<') {
               /* make sure previous token is word or dq token */
               size_t len = DynArray_getLength(oTokens);
               if (len == 0){
                  return LEX_ERR_PIPE_UNSPECIFIED;
               }
               psToken = DynArray_get(oTokens,len-1);
               if (psToken->eType != TOKEN_DQ && psToken->eType != TOKEN_WORD) {
                  return LEX_ERR_PIPE_UNSPECIFIED;
               }

               if (nbars) {
                  return LEX_ERR_PIPE_MULTIPLE;
               }

               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  return LEX_ERR_MEM;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  return LEX_ERR_MEM;
               }
               /* next add pipe token */
               acValue[0] = '<';
               acValue[1] = '\0';

               psToken = makeToken(TOKEN_PIPE, acValue);

               if (psToken == NULL)
               {
                  return LEX_ERR_MEM;
               }

               if (! DynArray_add(oTokens, psToken))
               {
                  return LEX_ERR_MEM;
               }

               iValueIndex = 0;
               eState = STATE_START;
               nstdinr++;
            }

            else if (isprint(c))
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_WORD;
            }

            else
            {
               return LEX_ERR_NPRINT;
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

void free_token_dynarr_preserve(DynArray_T tokens) {
   DynArray_map(tokens, freeToken_preserve_pcvalue, NULL);
   DynArray_free(tokens);
}



/* tokenize a command line passed from input */
int tokenize (char *cmdline, DynArray_T *token_ptr) {

   DynArray_T oTokens;
   enum LexResult result;
   
   oTokens = DynArray_new(0);
   if (oTokens == NULL)
   {
      fprintf(stderr, "Cannot allocate memory\n");
      exit(EXIT_FAILURE);
   }

   result = lexLine(cmdline, oTokens);
   if (result != LEX_SUCCESS) {
      /* free resources to prevent memory leak */
      free_token_dynarr(oTokens);
      /* print appropriate error message */
      switch (result) {
         case LEX_ERR_PIPE_MULTIPLE:
            LogErr("Multiple redirection of standard in/out");
         break;

         case LEX_ERR_PIPE_UNSPECIFIED:
            LogErr("Pipe or redirection destination not specified");
         break;

         case LEX_ERR_QUOTE:
            LogErr("Could not find quote pair");
         break;

         case LEX_ERR_MEM:
            LogErr("Out of memory");
         break;

         case LEX_ERR_NPRINT:
            LogErr("Use of non-printable chars");
         break;

         default:
            assert(FALSE);
         break;
      }
      return 0;
   } else {
      /* do lexical check on argv */
      result = lexChk(oTokens);

      switch (result) {
         case LEX_SUCCESS:
            *token_ptr = oTokens;
            return 1;
         break;

         case LEX_ERR_PIPE_MULTIPLE:
            LogErr("Multiple redirection of standard in/out");
         break;

         case LEX_ERR_PIPE_UNSPECIFIED:
            LogErr("Pipe or redirection destination not specified");
         break;

         case LEX_ERR_QUOTE:
            LogErr("Could not find quote pair");
         break;

         case LEX_ERR_MEM:
            LogErr("Out of memory");
         break;

         case LEX_ERR_NPRINT:
            LogErr("Use of non-printable chars");
         break;

         case LEX_ERR_PIPE_FNAME_INVALID:
            LogErr("Filename consists of multiple tokens");
         break;


         default:
            assert(FALSE);
         break;
      }

      /* free resources */
      DynArray_map(oTokens, freeToken_preserve_pcvalue, NULL);
      DynArray_free(oTokens);
      return 0;    

   }
}
