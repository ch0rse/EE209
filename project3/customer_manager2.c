#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define INITIAL_BUCKET_CNT 0x400
#define HASH_MULTIPLIER 65599

struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  unsigned int purchase;              // purchase amount (> 0)
  struct UserInfo *next_id;
  struct UserInfo *prev_id;
  struct UserInfo *next_name;
  struct UserInfo *prev_name;
};


struct DB {
  struct UserInfo **hashtable_id;   // pointer to the array
  struct UserInfo **hashtable_name;
  unsigned int numItems;
  unsigned int bucketCount;
};

static unsigned int hash_function(const char *pcKey, int iBucketCount)

/* Return a hash code for pcKey that is between 0 and iBucketCount-1,
   inclusive. Adapted from the EE209 lecture notes. */
{
   int i;
   unsigned int uiHash = 0U;
   for (i = 0; pcKey[i] != '\0'; i++)
      uiHash = uiHash * (unsigned int)HASH_MULTIPLIER
               + (unsigned int)pcKey[i];
   return (uiHash % (unsigned int)iBucketCount);
}

static struct UserInfo *find_user_by_id (DB_T d, const char *id) {

  unsigned int hash = hash_function (id, d->bucketCount);
  struct UserInfo *head = d->hashtable_id[hash]->next_id;

  while (head) {
    if (!strcmp (head->id, id)) {
      return head;
    }
    head = head->next_id;
  }

  return NULL;

}

static struct UserInfo *find_user_by_name (DB_T d, const char *name) {

  unsigned int hash = hash_function (name, d->bucketCount);
  struct UserInfo *head = d->hashtable_name[hash]->next_name;

  while (head) {
    if (!strcmp (head->name, name)) {
      return head;
    }
    head = head->next_name;
  }

  return NULL;
}

static int list_iterate_id (struct UserInfo *head, FUNCPTR_T fp) {

  int retval = 0;

  while (head) {
    retval += fp (head->id, head->name, head->purchase);
    head = head->next_id;
  }

  return retval;
}


/*--------------------------------------------------------------------*/
DB_T
CreateCustomerDB(void)
{
  DB_T d;
  unsigned int i;
  struct UserInfo *head;
  
  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }

  d->bucketCount = INITIAL_BUCKET_CNT;
  d->numItems = 0;

  d->hashtable_id = (struct UserInfo **)calloc (d->bucketCount, sizeof(struct UserInfo *));

  if (d->hashtable_id == NULL) {
    fprintf(stderr, "Can't allocate a memory for hash table of bucket size %u\n", d->bucketCount);   
    free (d);
    return NULL;
  }

  d->hashtable_name = (struct UserInfo **)calloc (d->bucketCount, sizeof(struct UserInfo *));

  if (d->hashtable_name == NULL) {
    fprintf(stderr, "Can't allocate a memory for hash table of bucket size %u\n", d->bucketCount);   
    free (d->hashtable_id);
    free (d);
    return NULL;
  }

  for (i = 0; i< d->bucketCount; i++) {
    head = (struct UserInfo *)calloc (1, sizeof(struct UserInfo));
    if (!head) {
      fprintf(stderr, "Can't allocate a new entry\n");   
      DestroyCustomerDB (d);
      return NULL;
    }

    d->hashtable_id[i] = head;
  }

  for (i = 0; i< d->bucketCount; i++) {
    head = (struct UserInfo *)calloc (1, sizeof(struct UserInfo));
    if (!head) {
      fprintf(stderr, "Can't allocate a new entry\n");   
      DestroyCustomerDB (d);
      return NULL;
    }

    d->hashtable_name[i] = head;
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

  /* free all entries */

  if (d->hashtable_id) {
    free (d->hashtable_id);
  }

  if (d->hashtable_name) {
    free (d->hashtable_name);
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
    fprintf(stderr, "RegisterCustomer: null argument\n");
    return -1;
  }

  /* check if user already exists */
  if (find_user_by_id (d, id)) {
    fprintf(stderr, "Attempt to add a user that already exists\n");
    return -1;
  }

  if (find_user_by_name (d, name)) {
    fprintf(stderr, "Attempt to add a user that already exists\n");
    return -1;
  }

  /* hashtable expansion */

  /* add new element */
  char *id_cpy, *name_cpy;
  struct UserInfo *new_user;

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

  new_user = calloc (1, sizeof (struct UserInfo));

  if (!new_user) {
    fprintf(stderr, "Can't allocate a memory for new user\n"); 
    return -1;
  }

  new_user->id = id_cpy;
  new_user->name = name_cpy;
  new_user->purchase = purchase;

  /* push to front of list in hashtable_id*/
  unsigned int hash = hash_function (id, d->bucketCount);
  struct UserInfo *head = d->hashtable_id[hash];

  new_user->next_id = head->next_id;
  new_user->prev_id = head;
  

  if (head->next_id) {
    head->next_id->prev_id = new_user;
  }

  head->next_id = new_user;

  /* do it for hashtable_name as well */
  hash = hash_function (name, d->bucketCount);
  head = d->hashtable_name[hash];
  
  new_user->next_name = head->next_name;
  new_user->prev_name = head;
  

  if (head->next_name) {
    head->next_name->prev_name = new_user;
  }

  head->next_name = new_user;
  /* increment numItems */
  d->numItems++;

  return 0;
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

  /* find UserInfo struct with id */
  struct UserInfo *victim = find_user_by_id (d, id);

  if (!victim) {
    fprintf(stderr,"Customer with ID %s was not found\n",id);
    return -1;
  }

  /* remove from linked list */
  if (victim->prev_id) {
    victim->prev_id->next_id = victim->next_id;
  }

  if (victim->next_id) {
    victim->next_id->prev_id = victim->prev_id;
  }

  if (victim->prev_name) {
    victim->prev_name->next_name = victim->next_name;
  }

  if (victim->next_name) {
    victim->next_name->prev_name = victim->prev_name;
  }

  /* free structure */
  free (victim->id);
  free (victim->name);
  free (victim);

  d->numItems--;

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

  /* find UserInfo struct with name */
  struct UserInfo *victim = find_user_by_name (d, name);

  if (!victim) {
    fprintf(stderr,"Customer with name %s was not found\n",name);
    return -1;
  }

  /* remove from linked list */
  if (victim->prev_id) {
    victim->prev_id->next_id = victim->next_id;
  }

  if (victim->next_id) {
    victim->next_id->prev_id = victim->prev_id;
  }

  if (victim->prev_name) {
    victim->prev_name->next_name = victim->next_name;
  }

  if (victim->next_name) {
    victim->next_name->prev_name = victim->prev_name;
  }

  /* free structure */
  free (victim->id);
  free (victim->name);
  free (victim);

  /*decrement numItems */
  d->numItems--;

  return 0;
}
/*--------------------------------------------------------------------*/
int
GetPurchaseByID(DB_T d, const char* id)
{
  /* return error if d == NULL */
  if (!d || !id) {
    fprintf(stderr, "GetPurchaseByID: null argument\n");
    return -1;
  }

  /* find UserInfo struct with id */
  struct UserInfo *victim = find_user_by_id (d, id);

  if (!victim) {
    fprintf(stderr,"Customer with ID %s was not found\n",id);
    return -1;
  }

  return victim->purchase;
}
/*--------------------------------------------------------------------*/
int
GetPurchaseByName(DB_T d, const char* name)
{
  /* return error if d == NULL */
  if (!d || !name) {
    fprintf(stderr, "GetPurchaseByName: null argument\n");
    return -1;
  }

  /* find UserInfo struct with name */
  struct UserInfo *victim = find_user_by_name (d, name);

  if (!victim) {
    fprintf(stderr,"Customer with name %s was not found\n",name);
    return -1;
  }

  return victim->purchase;
}
/*--------------------------------------------------------------------*/
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  /* return error if d == NULL */
  if (!d || !fp) {
    fprintf(stderr, "GetSumCustomerPurchase: null argument\n");
    return -1;
  }

  /* iterate every list */
  unsigned int i;
  int sum = 0;

  for (i = 0; i < d->bucketCount; i++) {
    if (d->hashtable_id[i]) {
      sum += list_iterate_id (d->hashtable_id[i]->next_id, fp);
    }
  }

  return sum;
}
