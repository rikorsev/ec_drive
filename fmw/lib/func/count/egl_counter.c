#include <assert.h>
#include "egl_counter.h"

void egl_counter_init(egl_counter_t* ctr)
{
  assert(ctr->get);
  assert(ctr->update);

  if(ctr->init)
    {
      ctr->init();
    }
}

uint32_t egl_counter_get(egl_counter_t* ctr)
{
  assert(ctr->get);
  return ctr->get();
}

void egl_counter_update(egl_counter_t* ctr)
{
  assert(ctr->update);
  ctr->update();
}

void egl_counter_set(egl_counter_t* ctr, uint32_t val)
{
  assert(ctr->set);
  ctr->set(val);
}

void egl_counter_deinit(egl_counter_t* cnt)
{
  if(cnt->deinit)
    {
      cnt->deinit();
    }
}
