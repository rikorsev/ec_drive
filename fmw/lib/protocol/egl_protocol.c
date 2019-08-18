#define EGL_MODULE_NAME "PTC"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "egl_result.h"
#include "egl_protocol.h"
#include "egl_trace.h"

egl_result_t egl_ptc_decode(egl_ptc_t *ptc, uint8_t *in, size_t *len_in, uint8_t *out, size_t *len_out)
{
  egl_result_t result = EGL_FAIL;
  size_t offset_in    = 0;
  size_t offset_out   = 0;
  size_t remain_in    = *len_in;
  size_t remain_out   = *len_out;

  assert(ptc         != NULL);
  assert(in          != NULL);
  assert(out         != NULL);
  assert(len_in      != NULL);
  assert(len_out     != NULL);
  assert(ptc->decode != NULL);
  assert(ptc->handle != NULL);
  assert(ptc->meta   != NULL);
  
  while(*len_in - offset_in > 0 && *len_out - offset_out > 0)
    {
      remain_in = *len_in - offset_in;
      result = ptc->decode(ptc->meta, in + offset_in, &remain_in);
      offset_in  += remain_in;
      
      if(result == EGL_SUCCESS)
      {
        remain_out = *len_out - offset_out;
        result = ptc->handle(ptc->meta, out + offset_out, &remain_out);
        offset_out += remain_out;
      }
    }

  *len_in = offset_in;
  *len_out = offset_out;
  
  return result;
}

