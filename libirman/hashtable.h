/* hashtable.h 0.1 1999/1/30 Tom Wheeley <tomw@tsys.demon.co.uk> */
/* This code is placed under the GNU Public Licence              */

#include <stdlib.h>
#include "chunk.h"

#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef struct ht_entry_s {
  char *key;
  void *data;
  struct ht_entry_s *next;
} ht_entry_t;

typedef struct ht_s {
  size_t size;
  chunk_t *chunk;
  ht_entry_t **table;
} ht_t;

ht_t *ht_new(size_t size);
int ht_add(char *key, void *data, ht_t *ht);
void *ht_match(char *key, ht_t *ht);
int ht_remove(char *key, ht_t *ht);
int ht_update(char *key, void *data, ht_t *ht);
void ht_free(ht_t **ht);

#endif /* HASHTABLE_H */

/* end of hashtable.h */
