#ifndef EGL_INTERFACE_H
#define EGL_INTERFACE_H

#include "stdint.h"
#include "stdlib.h"

typedef struct
{
  void             (*init)          (void);
  egl_result_t     (*open)          (void);
  size_t           (*write)         (void* data, size_t len);
  size_t           (*read)          (void* data, size_t len);
  egl_result_t     (*ioctl)         (uint8_t opcode, void* data, size_t len);
  egl_result_t     (*close)         (void);
  void             (*deinit)        (void);
}egl_interface_t;

void             egl_itf_init          (egl_interface_t* itf);
egl_result_t     egl_itf_open          (egl_interface_t* itf);
egl_result_t     egl_itf_write         (egl_interface_t* itf, void* buff, size_t* len);
void             egl_itf_write_byte    (egl_interface_t* itf, uint8_t data);
void             egl_itf_write_halfword(egl_interface_t* itf, uint16_t data);
void             egl_itf_write_word    (egl_interface_t* itf, uint32_t data);
void             egl_itf_write_longword(egl_interface_t* itf, uint64_t data);
egl_result_t     egl_itf_ioctl         (egl_interface_t* itf, uint8_t opcode, void* data, size_t len);
egl_result_t     egl_itf_read          (egl_interface_t* itf, void* buff, size_t* len);
uint8_t          egl_itf_read_byte     (egl_interface_t* itf);
uint16_t         egl_itf_read_halfword (egl_interface_t* itf);
uint32_t         egl_itf_read_word     (egl_interface_t* itf);
uint64_t         egl_itf_read_longword (egl_interface_t* itf);
egl_result_t egl_itf_close             (egl_interface_t* itf);

#endif // EGL_INTERFACE_H
