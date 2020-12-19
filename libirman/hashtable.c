/* hashtable.c 0.1 1999/1/16 Tom Wheeley <tomw@tsys.demon.co.uk> */
/* This code is placed under the GNU Public Licence              */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "chunk.h"
#include "hashtable.h"

static size_t ht_hash_code(char *key, unsigned max)
{
  register unsigned long g;
  register unsigned long h;
 
  if (!key) return 0;

  h = 0;
  while (*key) {
    h = (h << 4) + *key++;
    if ((g = h & 0xf0000000))
      h ^= g >> 24;
    h &= ~g;
  }
 
  return h % max;
}


ht_t *ht_new(size_t size)
{
  ht_t *ht;
  
  if (size <= 0)
    return NULL;
  
  ht = malloc(sizeof (ht_t));
  if (!ht)
    return NULL;
    
  ht->size = size;
  ht->table = calloc(size, sizeof (ht_entry_t *));
  if (!ht->table) {
    free(ht);
    return NULL;
  }
  ht->chunk = ch_new(size * sizeof (ht_entry_t));
  if (!ht->chunk) {
    free(ht->table);
    free(ht);
    return NULL;
  }

  return ht;
}

int ht_add(char *key, void *data, ht_t *ht)
{
  size_t hashval;
  ht_entry_t *node;
  
  if (!ht || !ht->table || ht->size <= 0) {
    errno = ENOMEM;
    return -1;
  }

  hashval = ht_hash_code(key, ht->size);

  /* check to see if this already exists.   Waste of time?  If we remove this
   * check then a sort of `stack' would be created.  Hmm, sounds good
   */
   
 #if 0
  for (node=ht->table[hashval]; node; node=node->next) {
    if (!strcmp(key, node->key)) {
      errno = EBUSY; /* key in use */
      return -1;
    }
  }
#endif

  node = ch_malloc(sizeof (ht_entry_t), ht->chunk);
  if (!node) {
    return -1;
  }
  
  /* add to the beginning of the chain - easiest way and gives us this
   * nifty stack type mode
   */ 
  node->key = key;
  node->data = data;
  node->next = ht->table[hashval];
  ht->table[hashval] = node;

  return 0;
}

void *ht_match(char *key, ht_t *ht)
{
  ht_entry_t *node;
  unsigned hashval;

  if (!ht || !ht->table) {
    return NULL;
  }

  hashval = ht_hash_code(key, ht->size);
  for (node = ht->table[hashval]; node; node = node->next) {
    if (!strcmp(key, node->key)) {
      return node->data;
    }
  }

  errno = ENOENT;    
  return NULL;
}


int ht_remove(char *key, ht_t *ht)
{
  ht_entry_t **pn;
  unsigned hashval;
  
  if (!ht || !ht->table) {
    return -1;
  }
  
  hashval = ht_hash_code(key, ht->size);
  for (pn = &(ht->table[hashval]); *pn; pn = &((*pn)->next)) {
    if (!strcmp(key, (*pn)->key)) {
      (*pn) = (*pn)->next;
      return 0;
    }
  }

  errno = ENOENT;
  return -1;
}


int ht_update(char *key, void *data, ht_t *ht)
{
  while (ht_match(key, ht) != NULL) {
    ht_remove(key, ht);
  }

  return ht_add(key, data, ht);
}


void ht_free(ht_t **ht)
{
  if (!ht || !(*ht))
    return;
    
  if ((*ht)->table)
    free((*ht)->table);
  if ((*ht)->chunk)
    ch_free((*ht)->chunk);
    
  free((*ht));
  (*ht) = NULL;
}


/* end of hashtable.c */
