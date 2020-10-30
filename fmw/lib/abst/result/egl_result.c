#include <assert.h>

#include "egl_result.h"

static char *egl_result_str[] =
{
  "EGL_SUCCESS",
  "EGL_FAIL",
  "EGL_SET",
  "EGL_RESET",
  "EGL_PROCESS",
  "EGL_NOT_SUPPORTED",
  "EGL_OUT_OF_BOUNDARY",
  "EGL_INVALID_PARAM",
  "EGL_INVALID_STATE"
};

char *egl_result_str_get(egl_result_t result)
{
  assert(result < EGL_LAST);

  return egl_result_str[result];
}
