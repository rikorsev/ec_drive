#ifndef EGL_LED_H
#define EGL_LED_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
  void (*init)(void);
  void (*on)(void);
  void (*off)(void);
  bool (*state)(void);
  void (*deinit)(void);
} egl_led_itf_t;

typedef struct
{
  egl_led_itf_t itf;
  egl_counter_t* ctr;
} egl_led_t;

void egl_led_init        (egl_led_t* led);
void egl_led_on          (egl_led_t* led);
void egl_led_off         (egl_led_t* led);
void egl_led_toggle      (egl_led_t* led);
bool egl_led_state       (egl_led_t* led);
void egl_led_deinit      (egl_led_t* led);
void egl_led_set_counter (egl_led_t* led, egl_counter_t* ctr);
void egl_led_blink       (egl_led_t* led, uint32_t period);
void egl_led_pulse       (egl_led_t* led, uint32_t on_period, uint32_t off_period);

#endif // EGL_LED_H
