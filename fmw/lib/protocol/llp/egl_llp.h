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

typedef struct
{
  egl_llp_pack_t pack;
  size_t buff_size;
  egl_crc_t *crc;
}egl_llp_data_t;

typedef egl_result_t (*egl_llp_handler_t)(const void *in, size_t len_in, void *out, size_t *len_out);

typedef struct
{
  uint16_t          id;
  egl_llp_handler_t handler;
}egl_llp_req_t;

typedef struct
{
  egl_llp_meta_state_t state;
  egl_llp_req_t        *req_map;
  size_t               req_map_len;
  size_t               count;
  egl_llp_data_t       in;
  egl_llp_data_t       out;
}egl_llp_t;

#define EGL_LLP_DECLARE(NAME, MAP, CRC_IN, CRC_OUT, IN_BUFF_SIZE, OUT_BUFF_SIZE) \
static uint8_t llp_in_buff_##NAME[(IN_BUFF_SIZE)] = {0};                  \
static uint8_t llp_out_buff_##NAME[(OUT_BUFF_SIZE)] = {0};                \
static egl_llp_t llp_##NAME =                                             \
{                                                                         \
  .state         = EGL_LLP_META_STATE_ID,                                 \
  .req_map       = (MAP),                                                 \
  .req_map_len   = sizeof(MAP),                                           \
  .count         = 0,                                                     \
  .in =                                                                   \
  {                                                                       \
    .pack.data = llp_in_buff_##NAME,                                      \
    .buff_size = (IN_BUFF_SIZE),                                          \
    .crc       = (CRC_IN),                                                \
  },                                                                      \
  .out =                                                                  \
  {                                                                       \
    .pack.data = llp_out_buff_##NAME,                                     \
    .buff_size = (OUT_BUFF_SIZE),                                         \
    .crc       = (CRC_OUT)                                                \
  }                                                                       \
};                                                                        \
egl_ptc_t NAME =                                                          \
{                                                                         \
  .meta   = &llp_##NAME,                                                  \
  .decode = egl_llp_decode,                                               \
  .handle = egl_llp_handle,                                               \
  .encode = egl_llp_encode,                                               \
  .setup  = egl_llp_setup                                                 \
};

egl_result_t egl_llp_decode(void *meta, uint8_t *raw, size_t *len);
egl_result_t egl_llp_handle(void *meta, uint8_t *out, size_t *len);
egl_result_t egl_llp_encode(void *meta, const void *data, size_t data_size, uint8_t *out, size_t *out_size);
egl_result_t egl_llp_setup(void *meta, const void *data, size_t len);

#endif
