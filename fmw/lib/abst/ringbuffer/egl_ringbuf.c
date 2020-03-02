#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "egl_ringbuf.h"

#define LIMIT_INDEX (-1)

egl_ringbuf_t* egl_ringbuf_create(size_t sz)
{
  uint8_t *pbuff;
  egl_ringbuf_t *ring;
  
  /* if buffer size is zero, so it is nothing to create */
  if(sz == 0)
  {
    return NULL;
  }

  /* Allocate memory for rinbugger structure */
  ring = malloc(sizeof(egl_ringbuf_t));
  if(ring == NULL) 
  {
    return NULL;
  }

  /* Allocate memory buffer itself */
  pbuff = malloc(sz);
  if(pbuff == NULL) 
  {
    free(ring);
    return NULL;
  }

  /* Init ringbuffer */
  ring->buff    = pbuff;
  ring->size    = sz;
  ring->wi      = 0;
  ring->ri      = 0;
  ring->overrun = false;

  return ring;
}

void egl_ringbuf_delete(egl_ringbuf_t *ring)
{
  free(ring->buff);
  free(ring);
}

uint8_t *egl_ringbuf_get_in_ptr(egl_ringbuf_t *ring)
{
  return &ring->buff[ring->wi];
}

uint8_t *egl_ringbuf_get_out_ptr(egl_ringbuf_t *ring)
{
  return &ring->buff[ring->ri];
}

size_t egl_ringbuf_get_cont_full_size(egl_ringbuf_t *ring)
{
  assert(ring != NULL);

  if(ring->ri > ring->wi)
  {
    return ring->size - ring->ri;
  }
  else if(ring->ri == ring->wi)
  {
    return ring->overrun == true ? ring->size - ring->ri : ring->ri;
  }
  else
  {
    return ring->wi - ring->ri;
  }
}

size_t egl_ringbuf_get_cont_free_size(egl_ringbuf_t *ring)
{
  assert(ring != NULL);

  return ring->size - egl_ringbuf_get_cont_full_size(ring);
}

static inline size_t inc_idx(size_t idx, size_t size, size_t limit)
{
  if(idx + size < limit)
  {
    return idx + size;
  }
  else
  {
    return idx + size - limit;
  }
}

static inline void inc_in_idx(egl_ringbuf_t *ring, size_t size)
{
  ring->wi = inc_idx(ring->wi, size, ring->size);

  /* Check if overrun occures */
  if(ring->wi == ring->ri)
  {
    ring->overrun = true;
  }
}

static inline void inc_out_idx(egl_ringbuf_t *ring, size_t size)
{
  ring->ri = inc_idx(ring->ri, size, ring->size);
  
  /* Check if we read everething */
  if(ring->wi == ring->ri)
  {
    ring->overrun = false;
  }
}

static inline size_t truncate(egl_ringbuf_t *ring, size_t size, size_t limit)
{
  if(limit < size)
  {
    size = limit;
  }

  return size;
}

size_t egl_ringbuf_reserve_for_read(egl_ringbuf_t *ring, size_t size)
{
  assert(ring != NULL);
  
  size = truncate(ring, size, egl_ringbuf_get_full_size(ring));
  
  if(size > 0)
  {
    inc_out_idx(ring, size);
  }

  return size;
}

size_t egl_ringbuf_reserve_for_write(egl_ringbuf_t *ring, size_t size)
{
  assert(ring != NULL);
  
  size = truncate(ring, size, egl_ringbuf_get_free_size(ring));
  
  if(size > 0)
  {
    inc_in_idx(ring, size);
  }

  return size;
}

size_t egl_ringbuf_read(egl_ringbuf_t *ring, void *dis, size_t size)
{
  /* Truncete size of first chunk to size to read */
  size_t chunk_one_size = truncate(ring, size, egl_ringbuf_get_cont_full_size(ring));
  size_t chunk_two_size = 0;

  assert(ring != NULL);
  assert(dis != NULL);

  /* Read first chunk of data */
  memcpy(dis, egl_ringbuf_get_out_ptr(ring), chunk_one_size);
  inc_out_idx(ring, chunk_one_size);

  /* If we read not all then read second one chunk */
  if(size > chunk_one_size)
  {
    /* Truncate chunk two in case if free size less then data to read */
    chunk_two_size = truncate(ring, size - chunk_one_size, egl_ringbuf_get_cont_full_size(ring));

    /* Read second chunk of data */
    memcpy(dis + chunk_one_size, egl_ringbuf_get_out_ptr(ring), chunk_two_size);
    inc_out_idx(ring, chunk_two_size);
  }

  return chunk_one_size + chunk_two_size;
}

size_t egl_ringbuf_write(egl_ringbuf_t *ring, void *src, size_t size)
{
  /* Truncete size of first chunk to size to read */
  size_t chunk_one_size = truncate(ring, size, egl_ringbuf_get_cont_free_size(ring));
  size_t chunk_two_size = 0;

  assert(ring != NULL);
  assert(src != NULL);
  
  /* Write first chunk of data */
  memcpy(egl_ringbuf_get_in_ptr(ring), src, chunk_one_size);
  inc_in_idx(ring, chunk_one_size);

  /* If we wrote not all, then write second one chunk */
  if(size > chunk_one_size)
  {
    /* Truncate chunk two in case if free size less then data to read */
    chunk_two_size = truncate(ring, size - chunk_one_size, egl_ringbuf_get_cont_free_size(ring));

    /* Write to buffer */
    memcpy(egl_ringbuf_get_in_ptr(ring), src + chunk_one_size, chunk_two_size);
    inc_in_idx(ring, chunk_two_size);
  }

  return chunk_one_size + chunk_two_size;
}

size_t egl_ringbuf_get_size(egl_ringbuf_t *ring)
{
  assert(ring != NULL);
  return ring->size;
}

size_t egl_ringbuf_get_free_size(egl_ringbuf_t *ring)
{
  assert(ring != NULL);
  return ring->size - egl_ringbuf_get_full_size(ring);
}

size_t egl_ringbuf_get_full_size(egl_ringbuf_t *ring)
{
  assert(ring != NULL);

  if(ring->ri > ring->wi)
  {
    return ring->size - ring->ri + ring->wi;
  }
  else if (ring->ri == ring->wi)
  {
    return ring->overrun == true ? ring->size : 0;
  }
  else
  {
    return ring->wi - ring->ri;
  }
}