/* chunk.h 0.1 1999/1/30 Tom Wheeley <tomw@tsys.demon.co.uk> */
/* This code is placed under the GNU Public Licence         */

#ifndef CHUNK_H
#define CHUNK_H

#define CH_ALIGN(x)	(((x) + 3) & ~3)


typedef struct chunk_s {
  size_t size;
  size_t free;
  void *bottom;
  void *top;
  struct chunk_s *next;
} chunk_t;

chunk_t *ch_new(size_t size);
chunk_t *xch_new(size_t size);
void *ch_malloc(size_t numbytes, chunk_t *chunk);
void *xch_malloc(size_t numbytes, chunk_t *chunk);
void ch_free(chunk_t *chunk);
void xch_free(chunk_t *chunk);
int ch_stat(chunk_t *chunk, int *num_blocks_r, size_t *block_size_r,
				size_t *mem_used_r, size_t *mem_wasted_r);

#endif /* CHUNK_H_ */

/* end of chunk.h */
