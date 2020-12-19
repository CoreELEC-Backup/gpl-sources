/* chunk.c 0.1 (c) 1999/1/30 Tom Wheeley <tomw@tsys.demon.co.uk> */
/* This code is placed under the GNU Public Licence              */

/*
 * Chunks are designed to store large numbers of small static parts of
 * memory, for example hash table keys, symbol names in a symbol table,
 * linked list / tree nodes etc.
 */

/*
 * a chunk is stored as a series of blocks, each of which is packed with
 * several objects allocated using ch_malloc().  Maintenance of these blocks
 * is performed transparently, the user only calling ch_new() to start off
 * each chunk, ch_malloc() to allocate an object in that chunk, and finally
 * ch_free() to delete that chunk, and all the objects contained within it.
 */
 
/* do not use realloc() / free() on the pointers returned by ch_malloc()
 * your program will surely suffer a segmentation fault!
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include "chunk.h"

/*
 * ch_new
 *
 * creates a new chunk block.  Must be called first to initialise a block
 * for subsequent calls to ch_malloc()
 *
 * size is the size of the large blocks of memory the word's are stored
 * in.  No word can be larger than `size', although you shouldn't even
 * consider a word length approaching even a fraction of `size'.
 */  
 
chunk_t *ch_new(size_t size)
{
  chunk_t *ch;

  ch = malloc(sizeof (chunk_t));
  if (!ch)
    return NULL;
  
  ch->size = CH_ALIGN(size);
  
  ch->free = ch->size;
  ch->bottom = malloc(ch->size);
  ch->top = ch->bottom;
  ch->next = NULL;

  if ( ! ch->bottom) {
    free(ch);
    return NULL;
  }
  
  return ch;
}

/* version of ch_new() which will crash and burn if malloc() fails */
chunk_t *xch_new(size_t size)
{
  chunk_t *ptr;
  
  ptr = ch_new(size);
  if (!ptr) {
    fprintf(stderr, "fatal error: unable to allocate memory\n");
    exit(EXIT_FAILURE);
  }
  
  return ptr;
}



/*
 * ch_malloc
 *
 * use in place of malloc when allocating a small piece of memory.
 *
 */
 
void *ch_malloc(size_t numbytes, chunk_t *chunk)
{
  chunk_t *ch;
  void *ptr;

  if (!chunk) {
    errno = ENOMEM;
    return NULL;
  }
  
  numbytes = CH_ALIGN(numbytes);
  
  /*
   * we could just add a new chunk to the linked list that _is_ large
   * enough, but you shouldn't be putting objects of this size in chunks
   * anyway!  These things are designed for hundreds of objects
   */
  if (numbytes > chunk->size) {
    errno = E2BIG;
    return NULL;
  }
  

  /* iterate through the list of blocks looking for one with enough space
   * left.  If none, then create a new one at the end
   */
  for(ch = chunk; ch->free < numbytes; ch=ch->next) {
    if (!ch->next) {
      ch->next = ch_new(ch->size);
    }
    if (!ch->next) {
      return NULL;
    }
  }
  
  /* sanity check */
  assert(ch && numbytes <= ch->free);
  
  ptr = ch->top;
  ch->free -= numbytes;
  ch->top = ((char *) ch->top) + numbytes;

  return ptr;
}

/* version of ch_malloc() which will crash and burn if malloc() fails */
void *xch_malloc(size_t numbytes, chunk_t *chunk)
{
  void *ptr;
  
  ptr = ch_malloc(numbytes, chunk);
  if (!ptr) {
    fprintf(stderr, "fatal error: unable to allocate memory\n");
    exit(EXIT_FAILURE);
  }

  return ptr;
}



/*
 * ch_free
 *
 * frees an entire chunk in one go.
 *
 */

void ch_free(chunk_t *chunk)
{
   chunk_t *dead;
   
   /* loop through, removing the data and the linked list blocks */
   for (;chunk;) {
     dead = chunk;
     chunk = dead->next;
     if (dead->bottom)
       free(dead->bottom);
     if (dead)
       free(dead);
   }
}

/* simply calls ch_free.  here to complete the namespace */
void xch_free(chunk_t *chunk)
{
  ch_free(chunk);
}



int ch_stat(chunk_t *chunk, int *num_blocks_r, size_t *block_size_r,
				size_t *mem_used_r, size_t *mem_wasted_r)
{
  chunk_t *ch;
  int num = 0;
  size_t used = 0;
  size_t wasted = 0;
  
  if (!chunk)
    return -1;
  
  /* only work these things out if people want to know */
  
  if (num_blocks_r || mem_used_r || mem_wasted_r) {
    for (ch=chunk; ch; ch=ch->next) {
      num++;
      used += ch->size - ch->free;
      if (ch->next) {
        wasted += ch->free;
      }
    }
  }
  
  /* this way, if people don't care about a particular statistic they can
   * just pass NULL.  Also helps prevent segfaults
   */
   
  if (num_blocks_r)
    *num_blocks_r = num;
  if (block_size_r)
    *block_size_r = chunk->size;	/* only safe way to get this */
  if (mem_used_r)
    *mem_used_r = used;
  if (mem_wasted_r)
    *mem_wasted_r = wasted;
    
  return 0;
}

/* end of chunk.c */
