#define EGL_MODULE_NAME "PTC"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "egl_result.h"
#include "egl_protocol.h"
#include "egl_trace.h"

egl_result_t egl_ptc_decode(egl_ptc_t *ptc, uint8_t *data, size_t *len)
{
  egl_result_t result = EGL_FAIL;
  size_t offset = 0;
  size_t remain = *len;
  
  assert(ptc         != NULL);
  assert(data        != NULL);
  assert(len         != NULL);
  assert(ptc->decode != NULL);
  assert(ptc->handle != NULL);
  assert(ptc->meta   != NULL);
  
  while(*len - offset > 0)
    {
      remain = *len - offset;
      result = ptc->decode(ptc->meta, data + offset, &remain);
      
      if(result == EGL_SUCCESS)
	{
	  result = ptc->handle(ptc->meta);
	}
      offset += remain;
    }

  *len = offset;
  return result;
}

egl_result_t egl_ptc_encode(egl_ptc_t, uint8_t *encoded, size_t *len)
{
  assert(ptc         != NULL);
  assert(encoded     != NULL);
  assert(len         != NULL);
  assert(ptc->meta   != NULL);
}
