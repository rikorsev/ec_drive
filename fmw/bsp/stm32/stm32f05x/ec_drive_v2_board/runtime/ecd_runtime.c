#include <string.h>
#include "egl_lib.h"

static volatile uint32_t runtime_val = 0;

static uint32_t get(void)
{
  return runtime_val;
}

static void update(void)
{
  runtime_val++;
}

static void set(uint32_t val)
{
  runtime_val = val;
}

const egl_counter_t ecd_runtime_impl =
  {
    .init   = NULL,
    .get    = get,
    .update = update,
    .set    = set,
    .deinit = NULL
  };

egl_counter_t* ecd_runtime(void)
{
  return (egl_counter_t*)&ecd_runtime_impl;
}
