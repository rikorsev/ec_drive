#include <assert.h>
#include <stdint.h>
#include "egl_counter.h"

void egl_delay(egl_counter_t* ctr, uint32_t delay)
{
  uint32_t target_count = 0;
  assert(ctr);

  target_count = egl_counter_get(ctr) + delay;
				    
  while(target_count > egl_counter_get(ctr))
    {
      /* do nothing */
    }
}
