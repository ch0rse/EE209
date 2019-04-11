#include <assert.h> /* to use assert() */
#include <stdio.h>
#include "str.h"

/* Your task is: 
   1. Rewrite the body of "Part 1" functions - remove the current
      body that simply calls the corresponding C standard library
      function.
   2. Write appropriate comment per each function
*/

/* Part 1 */
/*------------------------------------------------------------------------*/
size_t StrGetLength(const char* pcSrc)
{
  const char *pcEnd;
  assert(pcSrc); /* NULL address, 0, and FALSE are identical. */
  pcEnd = pcSrc;
	
  while (*pcEnd) /* null character and FALSE are identical. */
    pcEnd++;

  return (size_t)(pcEnd - pcSrc);
}

/*------------------------------------------------------------------------*/
char *StrCopy(char *pcDest, const char* pcSrc)
{
  /* TODO: fill this function */
  unsigned long length = StrGetLength (pcSrc);
  unsigned long i = 0;

  for (i=0; i < length; i++) {
    pcDest[i] = pcSrc[i];
  }

  pcDest[length] = 0;

  return pcDest;
}

/*------------------------------------------------------------------------*/
int StrCompare(const char* pcS1, const char* pcS2)
{
  unsigned long len1 = StrGetLength (pcS1);
  unsigned long len2 = StrGetLength (pcS2);
  unsigned long cmplen = len2;
  unsigned long i = 0;

  if (len2 < len1) {
    cmplen = len1;
  }

  for (i=0; i < cmplen; i++) {
    if (pcS1[i]-pcS2[i]) {
      return pcS1[i]-pcS2[i];
    }
  }

  assert (len1 == len2);
  return 0;
}
/*------------------------------------------------------------------------*/
char *StrSearch(const char* pcHaystack, const char *pcNeedle)
{
  unsigned long haystack_len = StrGetLength (pcHaystack);
  unsigned long needle_len = StrGetLength (pcNeedle);
  unsigned long i,j;
  for (i=0; i < haystack_len - needle_len; i++) {
    for (j=0; j < needle_len; j++) {
      if (pcHaystack[i+j]!=pcNeedle[j]){
        break;
      }
    }

    if (j == needle_len) {
      char *ptr = (char *)pcHaystack;
      return &ptr[i];
    }

  }

  return NULL;
}
/*------------------------------------------------------------------------*/
char *StrConcat(char *pcDest, const char* pcSrc)
{
  unsigned long len1 = StrGetLength (pcDest);
  char *ptr = &pcDest[len1];
  StrCopy (ptr, pcSrc);
  return pcDest;
}
