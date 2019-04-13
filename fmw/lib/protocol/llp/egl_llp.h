#ifndef EGL_LLP_H
#define EGL_LLP_H

#include "egl_protocol.h"

typedef enum
  {
    EGL_LLP_META_STATE_ID,
    EGL_LLP_META_STATE_LEN,
    EGL_LLP_META_STATE_DATA,
    EGL_LLP_META_STATE_CHECKSUM,
    EGL_LLP_META_STATE_DONE
  }egl_llp_meta_state_t;

typedef struct
{
  uint16_t   id;
  uint16_t   len;
  void       *data;
  uint16_t   checksum;
}egl_llp_pack_t;

typedef egl_result_t (*egl_llp_handler_t)(void *data, size_t len);

typedef struct
{
  uint16_t          id;
  egl_llp_handler_t handler;
}egl_llp_req_t;

typedef struct
{
  egl_llp_meta_state_t state;
  egl_llp_pack_t       pack;
  egl_llp_req_t        *req_map;
  size_t               req_map_len;
  size_t               count;
  size_t               buff_size;
  egl_crc_t            *(*crc)(void);
}egl_llp_t;

#define EGL_LLP_DECLARE(a_name, a_map, a_crc_func, a_buff_size) \
static uint8_t llp_buff_##a_name[buff_size] = {0};              \
static egl_llp_t llp_##a_name =                                 \
  {                                                             \
    .state       = EGL_LLP_META_STATE_ID,                       \
    .pack.data   = llp_buff_##name                              \
    .buff_size   = a_buff_size,                                 \
    .req_map     = a_map,                                       \
    .req_map_len = sizeof(a_map),                               \
    .count       = 0,                                           \
    .crc         = a_crc_func                                   \
  };                                                            \
egl_ptc_t name =                                                \
  {                                                             \
    .meta   = &llp_##a_name,                                    \
    .decode = egl_llp_decode,                                   \
    .handle = egl_llp_handle                                    \
  };

egl_result_t egl_llp_decode(void *meta, uint8_t *raw, size_t *len);
egl_result_t egl_llp_encode(void *meta, uint8_t *encoded, size_t *len);
egl_result_t egl_llp_handle(void *meta);

#endif
