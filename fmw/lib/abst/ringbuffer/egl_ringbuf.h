#ifndef EGL_RINGBUF_H
#define EGL_RINGBUF_H

#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  uint8_t* buff;
  size_t   size;
  size_t   ri;
  size_t   wi;
  bool     overrun;
}egl_ringbuf_t;

#define EGL_DECLARE_RINGBUF(name, sz)                \
static unsigned char rb_##name##_buff[(sz)] = {0};   \
egl_ringbuf_t name =                                 \
{                                                    \
  .buff      = rb_##name##_buff,                     \
  .size      = (sz),                                 \
  .ri        = 0,                                    \
  .wi        = 0,                                    \
  .overrun   = false                                 \
}

egl_ringbuf_t* egl_ringbuf_create(size_t sz);
void egl_ringbuf_delete(egl_ringbuf_t *ring);

size_t egl_ringbuf_read(egl_ringbuf_t *ring, void *dis, size_t sz);
size_t egl_ringbuf_write(egl_ringbuf_t *ring, void *src, size_t sz);

/* Usuall API */
size_t egl_ringbuf_get_size(egl_ringbuf_t *ring);
size_t egl_ringbuf_get_free_size(egl_ringbuf_t *ring);
size_t egl_ringbuf_get_full_size(egl_ringbuf_t *ring);
void egl_ringbuf_reset(egl_ringbuf_t *ring);
bool egl_ringbuf_is_empty(egl_ringbuf_t *ring);
bool egl_ringbuf_is_full(egl_ringbuf_t *ring);

/* API to work with ringbuffer directly. Useful in case to work with DMA */
uint8_t *egl_ringbuf_get_in_ptr(egl_ringbuf_t *ring);
uint8_t *egl_ringbuf_get_out_ptr(egl_ringbuf_t *ring);
size_t egl_ringbuf_reserve_for_write(egl_ringbuf_t *ring, size_t size);
size_t egl_ringbuf_reserve_for_read(egl_ringbuf_t *ring, size_t size);
size_t egl_ringbuf_get_cont_full_size(egl_ringbuf_t *ring);
size_t egl_ringbuf_get_cont_free_size(egl_ringbuf_t *ring);

#endif
