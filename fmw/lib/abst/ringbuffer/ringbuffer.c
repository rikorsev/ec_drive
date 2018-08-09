#include <assert.h>
#include <stdlib.h>
#include "ringbuffer.h"

/* TBD: macro initer */

ring_buffer_t* ring_buffer_create(size_t sz)
{
  char* pbuff;
  ring_buffer_t* prb;
  
  if(!sz) return NULL;

  prb=(ring_buffer_t*)malloc(sizeof(ring_buffer_t));

  if(!prb) return NULL;

  pbuff=(char*)malloc(sz);

  if(!pbuff) return NULL;

  prb->buff=pbuff;
  prb->size=sz;
  prb->idx_in=0;
  prb->idx_out=0;

  return prb;
  
}

void ring_buffer_delete(ring_buffer_t* prb)
{
  free(prb->buff);
  free(prb);
}

size_t ring_buffer_read(ring_buffer_t* prb, void* dis, size_t sz)
{
  size_t f_size;
  size_t idx;

  if(!prb) return 0;
  if(!dis) return 0;

  f_size = ring_buffer_get_full_size(prb);

  if(f_size<sz) sz = f_size;

  for(idx=0;idx<sz;idx++)
    {
      ((unsigned char*)dis)[idx]=((unsigned char*)prb->buff)[prb->idx_out];
      if(++(prb->idx_out)>=prb->size) prb->idx_out=0;
    }

  return sz;
}

size_t ring_buffer_write(ring_buffer_t* prb, void* src, size_t sz)
{
  size_t f_size;
  size_t idx;

  assert(prb);
  assert(src);
  
  f_size = ring_buffer_get_free_size(prb);

  if(f_size<sz) 
  {
    sz = f_size;
  }
  
  for(idx=0;idx<sz;idx++)
  {
    ((unsigned char*)prb->buff)[prb->idx_in] = ((unsigned char*)src)[idx];
    if(++(prb->idx_in)>=prb->size) 
    {
      prb->idx_in = 0;
    }
  }
  
  return sz;
}

size_t ring_buffer_get_size(ring_buffer_t* prb)
{
  assert(prb);
  return prb->size;
}

size_t ring_buffer_get_free_size(ring_buffer_t* prb)
{
  assert(prb);
  return prb->size - ring_buffer_get_full_size(prb) - 1;
}

size_t ring_buffer_get_full_size(ring_buffer_t* prb)
{
  size_t sz;
  assert(prb);

  if(prb->idx_out>prb->idx_in)
    {
      sz=prb->size-prb->idx_out+prb->idx_in;
    }
  else
    {
      sz=prb->idx_in-prb->idx_out;
    }
  return sz;
}
