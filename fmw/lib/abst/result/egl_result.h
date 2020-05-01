#ifndef EGL_RESULT_H
#define EGL_RESULT_H

typedef enum
  {
    EGL_SUCCESS,
    EGL_FAIL,
    EGL_SET,
    EGL_RESET,
    EGL_PROCESS,
    EGL_NOT_SUPPORTED,
    EGL_OUT_OF_BOUNDARY,
    EGL_INVALID_PARAM,
    EGL_INVALID_STATE,

    EGL_LAST
  }egl_result_t;

#define EGL_RESULT() (egl_result_str_get(result)) 

char *egl_result_str_get(egl_result_t result);
#endif
