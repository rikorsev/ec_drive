#define EGL_MODULE_NAME "LLP"

#include "stm32f0xx.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "egl_result.h"
#include "egl_crc.h"
#include "egl_llp.h"
#include "egl_trace.h"

static egl_llp_handler_t egl_llp_req_handler_get(uint16_t id, egl_llp_req_t *req_map, size_t req_map_len)
{
  egl_llp_handler_t handler = NULL;
    
  for(int i = 0; i < req_map_len; i++)
  {
    if(req_map[i].id == id)
    {
      handler = req_map[i].handler;
      break;
    }
  }

  return handler;
}

static size_t put_data(void *dst, size_t dst_size, uint8_t *src, size_t src_len, size_t count)
{
  size_t len = 0;

  if(count < dst_size)
  {
    len = dst_size - count;
    len = len < src_len ? len : src_len;
    memcpy(dst + count, src, len);
  }

  return len;
}

static egl_result_t decode_id(egl_llp_t *llp, uint8_t *raw, size_t *len)
{
  size_t decoded_len = put_data(&llp->in.pack.id,
                                sizeof(llp->in.pack.id),
                                raw, *len, llp->count);
  
  llp->count += decoded_len;
  *len = decoded_len;

  if(llp->count >= sizeof(llp->in.pack.id))
  {
    //EGL_TRACE_DEBUG("id: 0x%04x\r\n", llp->in.pack.id);
      
    llp->count = 0;
    if(egl_llp_req_handler_get(llp->in.pack.id,  llp->req_map, llp->req_map_len) != NULL)
    {
      llp->state = EGL_LLP_META_STATE_LEN;    
    }
    else
    {
      /* set that only one byte parset to pars id from the next byte next time */
      llp->in.pack.id >>= 8;
      llp->count = 1;
      *len = 1;
    }
  }
  
  return EGL_PROCESS;
}

static egl_result_t decode_len(egl_llp_t *llp, uint8_t *raw, size_t *len)
{
  egl_result_t result = EGL_PROCESS;
  size_t decoded_len = put_data(&llp->in.pack.len,
                                sizeof(llp->in.pack.len),
                                raw, *len, llp->count);
  
  llp->count += decoded_len;
  *len = decoded_len;
  
  if(llp->count >= sizeof(llp->in.pack.len))
  {
    //EGL_TRACE_DEBUG("len: 0x%04x\r\n", llp->in.pack.len);

    if(llp->in.pack.len <= llp->in.buff_size)
    {
      llp->state = EGL_LLP_META_STATE_DATA;
    }
    else
    {
      EGL_TRACE_ERROR("Packet len %d more then max %d\r\n", llp->in.pack.len,
      llp->in.buff_size);
      llp->state = EGL_LLP_META_STATE_ID;
      result = EGL_OUT_OF_BOUNDARY;
    }
      
    llp->count = 0;
  }
  
  return result;
}

static egl_result_t decode_data(egl_llp_t *llp, uint8_t *raw, size_t *len)
{
  size_t decoded_len = put_data(llp->in.pack.data,
                                llp->in.pack.len,
                                raw, *len, llp->count);
  
  llp->count += decoded_len;
  *len = decoded_len;
  
  if(llp->count <= llp->in.pack.len)
  {
    llp->count = 0;
    llp->state = EGL_LLP_META_STATE_CHECKSUM;
  }
  
  return EGL_PROCESS;
}

static egl_result_t decode_checksum(egl_llp_t *llp, uint8_t *raw, size_t *len)
{
  uint16_t checksum   = 0;
  egl_result_t result = EGL_PROCESS;
  size_t decoded_len  = put_data(&llp->in.pack.checksum,
                                 sizeof(llp->in.pack.checksum),
                                 raw, *len, llp->count);

  llp->count += decoded_len;
  *len = decoded_len;
  
  if(llp->count >= sizeof(llp->in.pack.checksum))
  {
    llp->count = 0;
    llp->state = EGL_LLP_META_STATE_ID;

    egl_crc_reset(llp->in.crc);
    checksum = egl_crc16_calc(llp->in.crc, &llp->in.pack.id, sizeof(llp->in.pack.id));
    checksum = egl_crc16_calc(llp->in.crc, &llp->in.pack.len, sizeof(llp->in.pack.len));
  
    if(llp->in.pack.data != NULL)
    {
      checksum = egl_crc16_calc(llp->in.crc, llp->in.pack.data, llp->in.pack.len);
    }
      
    //EGL_TRACE_DEBUG("CRC16: calc: 0x%04x, got: 0x%04x\r\n", checksum, llp->in.pack.checksum);
      
    result = checksum == llp->in.pack.checksum ? EGL_SUCCESS : EGL_FAIL ;
  }
  
  return result;
}

egl_result_t egl_llp_decode(void *meta, uint8_t *raw, size_t *len)
{
  egl_llp_t *llp = (egl_llp_t *) meta;
  egl_result_t result = EGL_FAIL;

  assert(meta != NULL);
  assert(raw  != NULL);
  assert(len  != NULL);
  
  switch(llp->state)
  {
    case EGL_LLP_META_STATE_ID:
      //EGL_TRACE_DEBUG("Decode ID\r\n");
      result = decode_id(llp, raw, len);
      break;
      
    case EGL_LLP_META_STATE_LEN:
      //EGL_TRACE_DEBUG("Decode length\r\n");
      result = decode_len(llp, raw, len);
      break;
      
    case EGL_LLP_META_STATE_DATA:
      //EGL_TRACE_DEBUG("Decode data\r\n");
      result = decode_data(llp, raw, len);
      break;
      
    case EGL_LLP_META_STATE_CHECKSUM:
      //EGL_TRACE_DEBUG("Decode checksum\r\n");
      result = decode_checksum(llp, raw, len);
      break;
  
    default:
      EGL_TRACE_ERROR(" Wrong parcer state\r\n");
      assert(0);
  }

  return result;
}

static egl_result_t encode(egl_llp_t *llp, uint8_t *out, size_t *len)
{
  size_t offset    = 0;
  size_t pack_size = sizeof(llp->out.pack.id) +
                     sizeof(llp->out.pack.len) +
                     sizeof(llp->out.pack.checksum) +
                     llp->out.pack.len;
    
  assert(llp != NULL);
  assert(out != NULL);
  assert(len != NULL);
  
  if(pack_size > *len)
  {
    EGL_TRACE_ERROR("Packet len %d more then buff size %d\r\n", llp->out.pack.len, *len);
    return EGL_OUT_OF_BOUNDARY;
  }

  if(llp->out.pack.len > 0 && llp->out.pack.data == NULL)
  {
    EGL_TRACE_ERROR("Data len is %d but data is NULL\r\n", llp->out.pack.len);
    return EGL_INVALID_PARAM;
  }
  
  memset(out, 0, *len);

  /* Copy command id */
  memcpy(out, &llp->out.pack.id, sizeof(llp->out.pack.id));
  offset += sizeof(llp->out.pack.id);

  /* Copy data lenght */
  memcpy(out + offset, &llp->out.pack.len, sizeof(llp->out.pack.len));
  offset += sizeof(llp->out.pack.len);

  /* Copy data */
  memcpy(out + offset, llp->out.pack.data, llp->out.pack.len);
  offset += llp->out.pack.len;

  /* Calc CRC */
  egl_crc_reset(llp->out.crc);
  llp->out.pack.checksum = egl_crc16_calc(llp->out.crc, out, offset);

  /* Copy crc */
  memcpy(out + offset, &llp->out.pack.checksum, sizeof(llp->out.pack.checksum));
  offset += sizeof(llp->out.pack.checksum);

  *len = offset;

  return EGL_SUCCESS;
}

egl_result_t egl_llp_handle(void *meta, uint8_t *out, size_t *len)
{
  egl_result_t result       = EGL_NOT_SUPPORTED;
  egl_llp_t *llp            = (egl_llp_t *) meta;
  size_t out_len            = 0;
  egl_llp_handler_t handler = egl_llp_req_handler_get(llp->in.pack.id,
                                                      llp->req_map,
                                                      llp->req_map_len);

  if(handler != NULL)
  {
    result = handler(llp->in.pack.data, 
                     llp->in.pack.len, 
                     llp->out.pack.data, 
                     &out_len);
    if(result == EGL_SUCCESS)
    {
      llp->out.pack.id  = llp->in.pack.id;
      llp->out.pack.len = out_len;
      result            = encode(llp, out, len);
    }
  }
  else
  {
    EGL_TRACE_ERROR("No handler foind for command 0x%04x\r\n", llp->in.pack.id);
  }

  return result;
}

egl_result_t egl_llp_encode(void *meta, const void *data, size_t data_size, uint8_t *out, size_t *out_size)
{
  egl_llp_t *llp = (egl_llp_t *) meta;

  llp->out.pack.len  = data_size;
  llp->out.pack.data = (void *)data;

  return encode(llp, out, out_size);
}

egl_result_t egl_llp_setup(void *meta, const void *data, size_t len)
{
  egl_llp_t *llp = (egl_llp_t *) meta;

  llp->out.pack.id = (uint16_t) data;

  return EGL_SUCCESS;
}