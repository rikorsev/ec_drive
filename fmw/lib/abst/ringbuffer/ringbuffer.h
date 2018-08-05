#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>
#include <stdlib.h>

typedef struct
{
  void* buff;
  size_t size;
  size_t idx_in;
  size_t idx_out;
}ring_buffer_t;

#define DECLARE_RING_BUFFER(name, sz)                  \
  static unsigned char rb_##name##_buff[(sz)] = {0};   \
  ring_buffer_t name =				       \
  {                                                    \
    .buff      = rb_##name##_buff,                     \
    .size      = (sz),  	                       \
    .idx_in    = 0,				       \
    .idx_out   = 0,				       \
  };                                            

ring_buffer_t* ring_buffer_create(size_t sz);
void ring_buffer_delete(ring_buffer_t* prb);

size_t ring_buffer_read(ring_buffer_t* prb, void* dis, size_t sz);
size_t ring_buffer_write(ring_buffer_t* prb, void* src, size_t sz);

size_t ring_buffer_get_size(ring_buffer_t* prb);
size_t ring_buffer_get_free_size(ring_buffer_t* prb);
size_t ring_buffer_get_full_size(ring_buffer_t* prb);

#endif
