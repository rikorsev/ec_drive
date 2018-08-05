#include <assert.h>
#include "egl_counter.h"
#include "egl_led.h" 


void egl_led_init(egl_led_t* led)
{
  assert(led);
  assert(led->itf.init);
  assert(led->itf.on);
  assert(led->itf.off);
  assert(led->itf.state);
  assert(led->itf.deinit);

  led->itf.init();
}

void egl_led_on(egl_led_t* led)
{
  assert(led);
  assert(led->itf.on);

  led->itf.on();
}

void egl_led_off(egl_led_t* led)
{
  assert(led);
  assert(led->itf.off);

  led->itf.off();
}

bool egl_led_state(egl_led_t* led)
{
  assert(led);
  assert(led->itf.state);

  return led->itf.state();
}

void egl_led_toggle(egl_led_t* led)
{
  assert(led);

  egl_led_state(led) ? egl_led_off(led) : egl_led_on(led);
}

void egl_led_deinit(egl_led_t* led)
{
  assert(led);
  assert(led->itf.deinit);

  led->itf.deinit();
}

void egl_led_set_counter(egl_led_t* led, egl_counter_t* ctr)
{
  assert(led);
  assert(ctr);

  led->ctr = ctr;
}

void egl_led_blink(egl_led_t* led, uint32_t period)
{
  static uint32_t target_count = 0;

  assert(led);
  assert(led->ctr);
  assert(period);
  
  if(target_count == 0)
    {
      target_count = egl_counter_get(led->ctr) + period;
    }

  if(target_count == egl_counter_get(led->ctr))
    {
      target_count = 0;
      egl_led_toggle(led);
    }
}

void egl_led_pulse(egl_led_t* led, uint32_t on_period, uint32_t off_period)
{
  static uint32_t target_on_count  = 0;
  static uint32_t target_off_count = 0;

  assert(led);
  assert(led->ctr);
  assert(on_period);
  assert(off_period);
  
  if(target_on_count == 0)
    {
      target_on_count = egl_counter_get(led->ctr) + on_period;
    }

  if(target_off_count == 0)
    {
      target_off_count = egl_counter_get(led->ctr) + off_period;
    }

  if(target_on_count == egl_counter_get(led->ctr))
    {
      target_on_count = 0;
      egl_led_off(led);
    }

  if(target_off_count == egl_counter_get(led->ctr))
    {
      target_off_count = 0;
      egl_led_on(led);
    }
}
