#ifndef EGL_COUNTER_H
#define EGL_COUNTER_H

#include <stdint.h>

typedef struct
{
  void     (*init)   (void);
  uint32_t (*get)    (void);
  void     (*update) (void);
  void     (*set)    (uint32_t val);
  void     (*deinit) (void);
}egl_counter_t;

void     egl_counter_init   (egl_counter_t* ctr);
uint32_t egl_counter_get    (egl_counter_t* ctr);
void     egl_counter_update (egl_counter_t* cnt);
void     egl_counter_set    (egl_counter_t* cnt, uint32_t val);
void     egl_counter_deinit (egl_counter_t* cnt);

#endif //EGL_COUNTER_H
