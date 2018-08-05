#ifndef EGL_INTERFACE_H
#define EGL_INTERFACE_H

#include "stdint.h"
#include "stdlib.h"

typedef enum
  {
    EGL_ITF_SUCCESS,
    EGL_ITF_NOT_SUPPORTED,
  }egl_itf_status_t;

typedef struct
{
  void             (*init)          (void);
  egl_itf_status_t (*open)          (void);
  size_t           (*write)         (void* data, size_t len);
  size_t           (*read)          (void* data, size_t len);
  egl_itf_status_t (*ioctl)         (uint8_t opcode, void* data, size_t len);
  egl_itf_status_t (*close)         (void);
  void             (*deinit)        (void);
}egl_interface_t;

void             egl_itf_init  (egl_interface_t* itf);
egl_itf_status_t egl_itf_open  (egl_interface_t* itf);
egl_itf_status_t egl_itf_write (egl_interface_t* itf, void* buff, size_t* len);
egl_itf_status_t egl_itf_ioctl(egl_interface_t* itf, uint8_t opcode, void* data, size_t len);
egl_itf_status_t egl_itf_read  (egl_interface_t* itf, void* buff, size_t* len);
egl_itf_status_t egl_itf_close (egl_interface_t* itf);

#endif // EGL_INTERFACE_H
