#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "egl_ringbuf.h"

egl_ringbuf_t* egl_ringbuf_create(size_t sz)
{
  uint8_t *pbuff;
  egl_ringbuf_t *prb;
  
  /* if buffer size is zero, so it is nothing to create */
  if(sz == 0)
  {
    return NULL;
  }

  /* Allocate memory for rinbugger structure */
  prb = malloc(sizeof(egl_ringbuf_t));
  if(prb == NULL) 
  {
    return NULL;
  }

  /* Allocate memory buffer itself */
  pbuff = malloc(sz);
  if(pbuff == NULL) 
  {
    free(prb);
    return NULL;
  }

  /* Init ringbuffer */
  prb->buff    = pbuff;
  prb->size    = sz;
  prb->idx_in  = 0;
  prb->idx_out = 0;

  return prb;
}

void egl_ringbuf_delete(egl_ringbuf_t *prb)
{
  free(prb->buff);
  free(prb);
}

uint8_t *egl_ringbuf_get_in_ptr(egl_ringbuf_t *prb)
{
  return &prb->buff[prb->idx_in];
}

uint8_t *egl_ringbuf_get_out_ptr(egl_ringbuf_t *prb)
{
  return &prb->buff[prb->idx_out];
}

size_t egl_ringbuf_get_cont_free_size(egl_ringbuf_t *prb)
{
  assert(prb != NULL);

  if(prb->idx_out > prb->idx_in)
  {
    return prb->idx_out - prb->idx_in;
  }
  else
  {
    return prb->size - prb->idx_in;
  }
}

size_t egl_ringbuf_get_cont_full_size(egl_ringbuf_t *prb)
{
  assert(prb != NULL);

  if(prb->idx_out > prb->idx_in)
  {
    return prb->size - prb->idx_out;
  }
  else
  {
    return prb->idx_in - prb->idx_out;
  }
}

static inline size_t inc_idx(size_t idx, size_t size, size_t limit)
{
  if(idx + size <= limit)
  {
    return idx + size;
  }
  else
  {
    return idx + size - limit;
  }
}

static inline void inc_in_idx(egl_ringbuf_t *prb, size_t size)
{
  prb->idx_in = inc_idx(prb->idx_in, size, prb->size);
}

static inline void inc_out_idx(egl_ringbuf_t *prb, size_t size)
{
  prb->idx_out = inc_idx(prb->idx_out, size, prb->size);
}

static inline size_t truncate(egl_ringbuf_t *prb, size_t size, size_t limit)
{
  if(limit < size)
  {
    size = limit;
  }

  return size;
}

size_t egl_ringbuf_reserve_for_read(egl_ringbuf_t *prb, size_t size)
{
  assert(prb != NULL);
  
  size = truncate(prb, size, egl_ringbuf_get_full_size(prb));
  
  if(size > 0)
  {
    inc_out_idx(prb, size);
  }

  return size;
}

size_t egl_ringbuf_reserve_for_write(egl_ringbuf_t *prb, size_t size)
{
  assert(prb != NULL);
  
  size = truncate(prb, size, egl_ringbuf_get_free_size(prb));
  
  if(size > 0)
  {
    inc_in_idx(prb, size);
  }

  return size;
}

size_t egl_ringbuf_read(egl_ringbuf_t *prb, void *dis, size_t size)
{
  /* Truncete size of first chunk to size to read */
  size_t chunk_one_size = truncate(prb, size, egl_ringbuf_get_cont_full_size(prb));
  size_t chunk_two_size = 0;

  assert(prb != NULL);
  assert(dis != NULL);

  /* Read first chunk of data */
  memcpy(dis, egl_ringbuf_get_out_ptr(prb), chunk_one_size);
  inc_out_idx(prb, chunk_one_size);
  
  /* If we read not all then read second one chunk */
  if(size > chunk_one_size)
  {
    /* Truncate chunk two in case if free size less then data to read */
    chunk_two_size = truncate(prb, size - chunk_one_size, egl_ringbuf_get_cont_full_size(prb));
    
    /* Read second chunk of data */
    memcpy(dis + chunk_one_size, egl_ringbuf_get_out_ptr(prb), chunk_two_size);
    inc_out_idx(prb, chunk_two_size);
  }

  return chunk_one_size + chunk_two_size;
}

size_t egl_ringbuf_write(egl_ringbuf_t *prb, void *src, size_t size)
{
  /* Truncete size of first chunk to size to read */
  size_t chunk_one_size = truncate(prb, size, egl_ringbuf_get_cont_free_size(prb));
  size_t chunk_two_size = 0;

  assert(prb != NULL);
  assert(src != NULL);
  
  /* Write first chunk of data */
  memcpy(egl_ringbuf_get_in_ptr(prb), src, chunk_one_size);
  inc_in_idx(prb, chunk_one_size);

  printf("size: %d, c1: %d\r\n", size, chunk_one_size);

  /* If we wrote not all, then write second one chunk */
  if(size > chunk_one_size)
  {
    /* Truncate chunk two in case if free size less then data to read */
    chunk_two_size = truncate(prb, size - chunk_one_size, egl_ringbuf_get_cont_free_size(prb));

    printf("c2: %d\r\n", chunk_two_size);

    /* Write to buffer */
    memcpy(egl_ringbuf_get_in_ptr(prb), src + chunk_one_size, chunk_two_size);
    inc_in_idx(prb, chunk_two_size);
  }
  
  return chunk_one_size + chunk_two_size;
}

size_t egl_ringbuf_get_size(egl_ringbuf_t *prb)
{
  assert(prb != NULL);
  return prb->size;
}

size_t egl_ringbuf_get_free_size(egl_ringbuf_t *prb)
{
  assert(prb != NULL);
  return prb->size - egl_ringbuf_get_full_size(prb);
}

size_t egl_ringbuf_get_full_size(egl_ringbuf_t *prb)
{
  assert(prb != NULL);

  if(prb->idx_out > prb->idx_in)
  {
    return prb->size - prb->idx_out + prb->idx_in;
  }
  else
  {
    return prb->idx_in - prb->idx_out;
  }
}