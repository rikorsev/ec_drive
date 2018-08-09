#ifndef EGL_TRACE_H
#define EGL_TRACE_H

#include "egl_counter.h"

typedef enum
{
  EGL_TRACE_LEVEL_DEBUG,
  EGL_TRACE_LEVEL_INFO,
  EGL_TRACE_LEVEL_ERROR,
  EGL_TRACE_LEVEL_FAIL,
  EGL_TRACE_LEVEL_LAST
}egl_trace_level_t;

#ifndef EGL_MODULE_NAME
#define EGL_MODULE_NAME NULL
#endif

#define EGL_TRACE_DEBUG(fmt, ...) egl_trace(EGL_TRACE_LEVEL_DEBUG, EGL_MODULE_NAME, fmt, ##__VA_ARGS__)
#define EGL_TRACE_INFO(fmt, ...)  egl_trace(EGL_TRACE_LEVEL_INFO,  EGL_MODULE_NAME, fmt, ##__VA_ARGS__)
#define EGL_TRACE_ERROR(fmt, ...) egl_trace(EGL_TRACE_LEVEL_ERROR, EGL_MODULE_NAME, fmt, ##__VA_ARGS__)
#define EGL_TRACE_FAIL(fmt, ...)  egl_trace(EGL_TRACE_LEVEL_FAIL,  EGL_MODULE_NAME, fmt, ##__VA_ARGS__)

void egl_trace_init (egl_trace_level_t lvl, egl_counter_t* tstamp, char** filtr, size_t num);
void egl_trace      (egl_trace_level_t lvl, char* module, char* format, ...);

#endif //EGL_TRACE_H
