#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define UNIT_ARRAY_SIZE 0x400
#define ARRAY_INCR_STEP 0x100

struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  unsigned int purchase;              // purchase amount (> 0)
};

struct DB {
  struct UserInfo *pArray;   // pointer to the array
  unsigned int curArrSize;            // current array size (max # of elements)
  unsigned int numItems;              // # of stored items, needed to determine
			     // # whether the array should be expanded
			     // # or not
};


/*--------------------------------------------------------------------*/
DB_T
CreateCustomerDB(void)
{
  DB_T d;
  
  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  d->curArrSize = UNIT_ARRAY_SIZE; // start with 1024 elements
  d->pArray = (struct UserInfo *)calloc(d->curArrSize, sizeof (struct UserInfo));

  if (d->pArray == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %u\n",
	    d->curArrSize);   
    free(d);
    return NULL;
  }

  return d;
}
/*--------------------------------------------------------------------*/
void
DestroyCustomerDB(DB_T d)
{
  /* do nothing if d == NULL */
  if (!d) {
    return;
  }

  if (d->pArray) {
    free (d->pArray);
  }

  free (d);
}
/*--------------------------------------------------------------------*/
int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
{
  /* return error if d == NULL */
  if (!d || !id || !name) {
    return -1;
  }

  /* unreachable situation */
  if (d->curArrSize < d->numItems) {
    fprintf(stderr, "Corrupted DB, numItems (%u) is larger than  curArrSize (%u)\n", d->numItems, d->curArrSize);  
    exit (-1);
  }

  /* check if user already exists */
  unsigned int i;
  char *p, *q;

  for (i = 0; i < d->curArrSize; i++) {

    p = (d->pArray[i]).id;
    q = (d->pArray[i]).name;

    if (!p || !q) {
      /* empty */
      continue;
    }

    if (!strcmp (p, id) || !strcmp (q, name)) {
      fprintf(stderr, "Attempt to add a user that already exists\n");
      return -1; 
    }
  }

  /* check if array must be expanded, and expand if necessary */
  if (d->curArrSize == d->numItems) {
    
    void *old = (void *) d->pArray;
    unsigned old_size = d->curArrSize;

    d->curArrSize*= 2;

    if (d->curArrSize <= old_size) {
      fprintf(stderr, "Integer overflow, incrementing from %u to %u is not allowed\n", old_size, d->curArrSize); 
      return -1;
    }

    d->pArray = calloc (d->curArrSize, sizeof (struct UserInfo));

    if (!d->pArray) {
      fprintf(stderr, "Can't allocate a memory for array of size %u\n", d->curArrSize); 
      d->pArray = old;
      return -1;  
    }

    memcpy (d->pArray, old, old_size * sizeof (struct UserInfo));
  }

  /* add new element */
  char *id_cpy, *name_cpy;

  id_cpy = strdup (id);
  if (!id_cpy) {
    fprintf(stderr, "Can't allocate a memory for id string via strdup ()\n"); 
    return -1;
  }

  name_cpy = strdup (name);
  if (!name_cpy) {
    fprintf(stderr, "Can't allocate a memory for name string via strdup ()\n"); 
    return -1;
  }

  /* find first nonzero entry and increment numItems */

  for (i = 0; i < d->curArrSize; i++) {

    p = (d->pArray[i]).id;
    q = (d->pArray[i]).name;

    if (!p && !q) {
      (d->pArray[i]).id = id_cpy;
      (d->pArray[i]).name = name_cpy;
      (d->pArray[i]).purchase = purchase;
      d->numItems++;
      return 0;
    }
  }

  /* unreachable */
  fprintf(stderr, "unreachable code\n");
  return -1;  
}
/*--------------------------------------------------------------------*/
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  /* return error if d == NULL */
  if (!d || !id) {
    fprintf(stderr, "UnregisterCustomerByID: null argument\n");
    return -1;
  }

  /* unreachable situation */
  if (d->curArrSize < d->numItems) {
    fprintf(stderr, "Corrupted DB, numItems (%u) is larger than  curArrSize (%u)\n", d->numItems, d->curArrSize);  
    exit (-1);
  }

  /* find customer */
  unsigned int i;
  char *p, *q;

  for (i = 0; i < d->curArrSize; i++) {
    
    p = (d->pArray[i]).id;
    q = (d->pArray[i]).name;

    if (!p || !q) {
      /* empty */
      continue;
    }

    if (!strcmp (p, id)) {
      break;
    }
  }
  
  if (i == d->curArrSize) {
    fprintf(stderr,"Customer with ID %s was not found\n",id);
    return -1;
  }

  /* free resources */
  free (p);
  free (q);

  (d->pArray[i]).id = NULL;
  (d->pArray[i]).name = NULL;
  (d->pArray[i]).purchase = 0;

  return 0;
}

/*--------------------------------------------------------------------*/
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  /* return error if d == NULL */
  if (!d || !name) {
    fprintf(stderr, "UnregisterCustomerByName: null argument\n");
    return -1;
  }

  /* unreachable situation */
  if (d->curArrSize < d->numItems) {
    fprintf(stderr, "Corrupted DB, numItems (%u) is larger than  curArrSize (%u)\n", d->numItems, d->curArrSize);  
    exit (-1);
  }

  /* find customer */
  unsigned int i;
  char *p, *q;

  for (i = 0; i < d->curArrSize; i++) {
    
    p = (d->pArray[i]).id;
    q = (d->pArray[i]).name;

    if (!p || !q) {
      /* empty */
      continue;
    }

    if (!strcmp (q, name)) {
      break;
    }
  }
  
  if (i == d->curArrSize) {
    fprintf(stderr,"Customer with name %s was not found\n",name);
    return -1;
  }

  /* free resources */
  free (p);
  free (q);

  (d->pArray[i]).id = NULL;
  (d->pArray[i]).name = NULL;
  (d->pArray[i]).purchase = 0;

  return 0;
}
/*--------------------------------------------------------------------*/
unsigned int
GetPurchaseByID(DB_T d, const char* id)
{
  /* return error if d == NULL */
  if (!d || !id) {
    fprintf(stderr, "GetPurchaseByID: null argument\n");
    return -1;
  }

  /* unreachable situation */
  if (d->curArrSize < d->numItems) {
    fprintf(stderr, "Corrupted DB, numItems (%u) is larger than  curArrSize (%u)\n", d->numItems, d->curArrSize);  
    exit (-1);
  }

  /* find customer */
  unsigned int i;
  char *p, *q;

  for (i = 0; i < d->curArrSize; i++) {
    
    p = (d->pArray[i]).id;
    q = (d->pArray[i]).name;

    if (!p || !q) {
      /* empty */
      continue;
    }

    if (!strcmp (p, id)) {
      break;
    }
  }
  
  if (i == d->curArrSize) {
    fprintf(stderr,"Customer with ID %s was not found\n",id);
    return -1;
  }

  return (d->pArray[i]).purchase;
}
/*--------------------------------------------------------------------*/
unsigned int
GetPurchaseByName(DB_T d, const char* name)
{
  /* return error if d == NULL */
  if (!d || !name) {
    fprintf(stderr, "GetPurchaseByID: null argument\n");
    return -1;
  }

  /* unreachable situation */
  if (d->curArrSize < d->numItems) {
    fprintf(stderr, "Corrupted DB, numItems (%u) is larger than  curArrSize (%u)\n", d->numItems, d->curArrSize);  
    exit (-1);
  }

  /* find customer */
  unsigned int i;
  char *p, *q;

  for (i = 0; i < d->curArrSize; i++) {
    
    p = (d->pArray[i]).id;
    q = (d->pArray[i]).name;

    if (!p || !q) {
      /* empty */
      continue;
    }

    if (!strcmp (q, name)) {
      break;
    }
  }
  
  if (i == d->curArrSize) {
    fprintf(stderr,"Customer with name %s was not found\n",name);
    return -1;
  }

  return (d->pArray[i]).purchase;
}
/*--------------------------------------------------------------------*/
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  /* return error if d == NULL */
  if (!d || !fp) {
    fprintf(stderr, "GetPurchaseByID: null argument\n");
    return -1;
  }

  /* unreachable situation */
  if (d->curArrSize < d->numItems) {
    fprintf(stderr, "Corrupted DB, numItems (%u) is larger than  curArrSize (%u)\n", d->numItems, d->curArrSize);  
    exit (-1);
  }

  /* find customer */
  unsigned int i, r;
  int sum = 0;
  char *p, *q;

  for (i = 0; i < d->curArrSize; i++) {
    
    p = (d->pArray[i]).id;
    q = (d->pArray[i]).name;
    r = (d->pArray[i]).purchase;

    if (!p || !q) {
      /* empty */
      continue;
    }

    sum += fp (p, q, r);
  }
  
  return sum;
}
