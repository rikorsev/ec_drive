#define EGL_MODULE_NAME "LLP"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
      memcpy(dst, src, len);
    }

  return len;
}

static egl_result_t decode_id(egl_llp_t *llp, uint8_t *raw, size_t *len)
{
  size_t decoded_len = put_data(&llp->pack.id,
				sizeof(llp->pack.id),
				raw, *len, llp->count);
  
  llp->count += decoded_len;
  *len = decoded_len;

  if(llp->count <= sizeof(llp->pack.id))
    {
      EGL_TRACE_DEBUG("id: 0x%04x\r\n", llp->pack.id);
      
      llp->count = 0;
      if(egl_llp_req_handler_get(llp->pack.id,  llp->req_map, llp->req_map_len) != NULL)
	{
	  llp->state = EGL_LLP_META_STATE_LEN;	  
	}
    }
  
  return EGL_PROCESS;
}

static egl_result_t decode_len(egl_llp_t *llp, uint8_t *raw, size_t *len)
{
  egl_result_t result = EGL_PROCESS;
  size_t decoded_len = put_data(&llp->pack.len,
				sizeof(llp->pack.len),
				raw, *len, llp->count);
  
  llp->count += decoded_len;
  *len = decoded_len;
  
  if(llp->count <= sizeof(llp->pack.len))
    {
      EGL_TRACE_DEBUG("len: 0x%04x\r\n", llp->pack.len);

      if(llp->pack.len <= llp->buff_size)
	{
	  llp->state = EGL_LLP_META_STATE_DATA;
	}
      else
	{
	  EGL_TRACE_ERROR("Packet len %d more then max %d\r\n", llp->pack.len,
			                                        llp->buff_size);
	  llp->state = EGL_LLP_META_STATE_ID;
	  result = EGL_OUT_OF_BOUNDARY;
	}
      
      llp->count = 0;
    }
  
  return result;
}

static egl_result_t decode_data(egl_llp_t *llp, uint8_t *raw, size_t *len)
{
  size_t decoded_len = put_data(llp->pack.data,
				llp->pack.len,
				raw, *len, llp->count);
  
  llp->count += decoded_len;
  *len = decoded_len;
  
  if(llp->count <= llp->pack.len)
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
  size_t decoded_len  = put_data(&llp->pack.checksum,
				 sizeof(llp->pack.checksum),
				 raw, *len, llp->count);

  llp->count += decoded_len;
  *len = decoded_len;
  
  if(llp->count <= sizeof(llp->pack.checksum))
    {
      llp->count = 0;
      llp->state = EGL_LLP_META_STATE_ID;

      egl_crc_reset(llp->crc());
      checksum = egl_crc16_calc(llp->crc(), &llp->pack.id, sizeof(llp->pack.id));
      checksum = egl_crc16_calc(llp->crc(), &llp->pack.len, sizeof(llp->pack.len));
	
      if(llp->pack.data != NULL)
	{
	  checksum = egl_crc16_calc(llp->crc(), llp->pack.data, llp->pack.len);
	}
      
      EGL_TRACE_DEBUG("CRC16: calc: 0x%04x, got: 0x%04x\r\n", checksum, llp->pack.checksum);
      
      result = checksum == llp->pack.checksum ? EGL_SUCCESS : EGL_FAIL ;
    }
  
  return result;
}

egl_result_t egl_llp_decode(void *meta, uint8_t *raw, size_t *len)
{
  egl_llp_t *llp = (egl_llp_t *) meta;
  egl_result_t result = EGL_FAIL;

  assert(meta != NULL);
  assert(raw != NULL);
  assert(len != NULL);
  
  switch(llp->state)
    {
    case EGL_LLP_META_STATE_ID:
      EGL_TRACE_DEBUG("Decode ID\r\n");
      result = decode_id(llp, raw, len);
      break;
      
    case EGL_LLP_META_STATE_LEN:
      EGL_TRACE_DEBUG("Decode length\r\n");
      result = decode_len(llp, raw, len);
      break;
      
    case EGL_LLP_META_STATE_DATA:
      EGL_TRACE_DEBUG("Decode data\r\n");
      result = decode_data(llp, raw, len);
      break;
      
    case EGL_LLP_META_STATE_CHECKSUM:
      EGL_TRACE_DEBUG("Decode checksum\r\n");
      result = decode_checksum(llp, raw, len);
      break;
  
    default:
      EGL_TRACE_ERROR(" Wrong parcer state\r\n");
      assert(0);
    }

  return result;
}

egl_result_t egl_llp_encode(void *meta, uint8_t *encoded, size_t *len);
{
  egl_llp_t *llp   = (egl_llp_pack_t *)meta;
  size_t offset    = 0;
  size_t pack_size = sizeof(llp->pack.id) +
                     sizeof(llp->pack.len) +
                     sizeof(llp->pack.checksum) +
                     ppl->pack.len;
    
  assert(meta    != NULL);
  assert(encoded != NULL);
  assert(len     != NULL)
  
  if(pack_size > *len)
    {
      EGL_TRACE_ERROR("Packet len %d more then buff size %d\r\n", pack->len, *len);
      return EGL_OUT_OF_BOUNDARY;
    }

  if(llp->pack.len > 0 && lpp->pack.data == NULL)
    {
      EGL_TRACE_ERROR("Data len is % but data is NULL\r\n", llp->pack.len);
      return EGL_INVALID_PARAM;
    }
  
  memset(encoded, 0, *len);

  memcpy(encoded, &llp->pack.id, sizeof(llp->pack.id));
  offset += sizeof(pack->id);

  memcpy(encoded + offset, &llp->pack.len, sizeof(llp->pack.len));
  offset += sizeof(llp->pack.len);

  memcpy(encoded + offset, llp->pack.data, llp->pack.len);
  offset += pack->len;

  llp->pack.checksum = egl_crc16_calc(llp->crc(), llp->pack.data, llp->pack.len);
  memcpy(encoded + offset, &llp->pack.checksum, sizeof(llp->pack.checksum));
  offset += sizeof(llp->pack.checksum);

  *len = offset;

  return EGL_SUCCESS;
}

egl_result_t egl_llp_handle(void *meta)
{
  egl_result_t result       = EGL_FAIL;
  egl_llp_t *llp            = (egl_llp_t *) meta;
  egl_llp_handler_t handler = egl_llp_req_handler_get(llp->pack.id,
						      llp->req_map,
						      llp->req_map_len);

  if(handler != NULL)
    {
      result = handler(llp->pack.data, llp->pack.len);      
    }

  return result;
}
