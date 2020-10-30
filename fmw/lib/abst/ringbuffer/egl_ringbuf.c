#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "egl_ringbuf.h"
#include "egl_min_max.h"

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

size_t egl_ringbuf_get_cont_fill_size(egl_ringbuf_t *ring)
{
  assert(ring != NULL);

  if(ring->ri > ring->wi)
  {
    return ring->size - ring->ri;
  }
  else if(ring->ri < ring->wi) 
  {
    return ring->wi - ring->ri;
  }
  else
  {
    return ring->overrun == true ? ring->size - ring->ri : 0;
  }
}

size_t egl_ringbuf_get_cont_free_size(egl_ringbuf_t *ring)
{
  assert(ring != NULL);

  if(ring->ri > ring->wi)
  {
    return ring->ri - ring->wi;
  }
  else if(ring->ri < ring->wi) 
  {
    return ring->size - ring->wi;
  }
  else
  {
    return ring->overrun == true ? 0 : ring->size - ring->wi;
  }
}

static inline size_t inc_idx(size_t idx, size_t size, size_t limit)
{
  return (idx + size) % limit;
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
  
  ring->overrun = false;
}

size_t egl_ringbuf_reserve_for_read(egl_ringbuf_t *ring, size_t size)
{
  assert(ring != NULL);
  
  size = MIN(size, egl_ringbuf_get_fill_size(ring));
  
  if(size > 0)
  {
    inc_out_idx(ring, size);
  }

  return size;
}

size_t egl_ringbuf_reserve_for_write(egl_ringbuf_t *ring, size_t size)
{
  assert(ring != NULL);
  
  size = MIN(size, egl_ringbuf_get_free_size(ring));
  
  if(size > 0)
  {
    inc_in_idx(ring, size);
  }

  return size;
}

size_t egl_ringbuf_read(egl_ringbuf_t *ring, void *dis, size_t size)
{
  size_t chunk_one_size = 0;
  size_t chunk_two_size = 0;
  
  if(size > 0)
  {
    /* Truncete size of first chunk to size to read */
    chunk_one_size = MIN(size, egl_ringbuf_get_cont_fill_size(ring));
    uint8_t *out = egl_ringbuf_get_out_ptr(ring);

    assert(ring != NULL);
    assert(dis != NULL);

    /* Read first chunk of data */
    inc_out_idx(ring, chunk_one_size);
    memcpy(dis, out, chunk_one_size);

    /* If we read not all then read second one chunk */
    if(size > chunk_one_size)
    {
      /* Truncate chunk two in case if free size less then data to read */
      chunk_two_size = MIN(size - chunk_one_size, egl_ringbuf_get_cont_fill_size(ring));
     
      /* Read second chunk of data */
      out = egl_ringbuf_get_out_ptr(ring);
      inc_out_idx(ring, chunk_two_size);
      memcpy(dis + chunk_one_size, out, chunk_two_size);
    }
  }

  return chunk_one_size + chunk_two_size;
}

size_t egl_ringbuf_write(egl_ringbuf_t *ring, void *src, size_t size)
{
  size_t chunk_one_size = 0;
  size_t chunk_two_size = 0;
  
  if(size > 0)
  {
    /* Truncete size of first chunk to size to read */
    chunk_one_size = MIN(size, egl_ringbuf_get_cont_free_size(ring));
    uint8_t *in = egl_ringbuf_get_in_ptr(ring);

    assert(ring != NULL);
    assert(src != NULL);
    
    /* Write first chunk of data */
    inc_in_idx(ring, chunk_one_size);
    memcpy(in, src, chunk_one_size);

    /* If we wrote not all, then write second one chunk */
    if(size > chunk_one_size && ring->overrun != true)
    {
      /* Truncate chunk two in case if free size less then data to read */
      chunk_two_size = MIN(size - chunk_one_size, egl_ringbuf_get_cont_free_size(ring));
      
      /* Write to buffer */
      in = egl_ringbuf_get_in_ptr(ring);
      inc_in_idx(ring, chunk_two_size);
      memcpy(in, src + chunk_one_size, chunk_two_size);
    }
  }
  
  return chunk_one_size + chunk_two_size;
}

size_t egl_ringbuf_get_size(egl_ringbuf_t *ring)
{
  assert(ring != NULL);
  return ring->size;
}

size_t egl_ringbuf_get_fill_size(egl_ringbuf_t *ring)
{
  assert(ring != NULL);

  if(ring->ri > ring->wi)
  {
    return ring->size - ring->ri + ring->wi;
  }
  else if (ring->ri < ring->wi)
  {
    return ring->wi - ring->ri;
  }
  else
  {
    return ring->overrun == true ? ring->size : 0;
  }
}

size_t egl_ringbuf_get_free_size(egl_ringbuf_t *ring)
{
  assert(ring != NULL);
  return ring->size - egl_ringbuf_get_fill_size(ring);
}

void egl_ringbuf_reset(egl_ringbuf_t *ring)
{
  ring->wi = 0;
  ring->ri = 0;
}

bool egl_ringbuf_is_empty(egl_ringbuf_t *ring)
{
  return egl_ringbuf_get_fill_size(ring) == 0 ? true : false;
}

bool egl_ringbuf_is_full(egl_ringbuf_t *ring)
{
  return egl_ringbuf_get_free_size(ring) == 0 ? true : false;
}