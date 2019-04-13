#include "egl_result.h"

static char *egl_result_str[] =
  {
    "EGL_SUCCESS",
    "EGL_FAIL",
    "EGL_PROCESS",
    "EGL_NOT_SUPPORTED",
    "EGL_OUT_OF_BOUNDARY",
    "EGL_INVALID_PARAM"
  };

char *egl_result_str_get(egl_result_t result)
{
  return egl_result_str[result];
}
