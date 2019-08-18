#include <assert.h>

#include "egl_result.h"
#include "egl_interface.h"

void egl_itf_init(egl_interface_t* itf)
{
  assert(itf);
  assert(itf->init);
  assert(itf->open);
  assert(itf->close);

  itf->init();
}

egl_result_t egl_itf_open(egl_interface_t* itf)
{
  assert(itf);
  assert(itf->open);

  return itf->open();
}

egl_result_t egl_itf_write(egl_interface_t* itf, void* buff, size_t* len)
{
  assert(itf);
  assert(buff);

  if(itf->write == NULL)
  {
    return EGL_NOT_SUPPORTED;
  }

  if(*len > 0)
  {
    *len = itf->write(buff, *len);
  }

  return EGL_SUCCESS;
}

egl_result_t egl_itf_ioctl(egl_interface_t* itf, uint8_t opcode, void* data, size_t len)
{
  assert(itf);

  if(itf->ioctl == NULL)
    {
      return EGL_NOT_SUPPORTED;
    }

  return itf->ioctl(opcode, data, len);
}

egl_result_t egl_itf_read(egl_interface_t* itf, void* buff, size_t* len)
{
  assert(itf);
  assert(buff);

  if(itf->read == NULL)
  {
    return EGL_NOT_SUPPORTED;
  }
  
  if(*len > 0)
  {
    *len = itf->read(buff, *len);
  }

  return EGL_SUCCESS;
}

egl_result_t egl_itf_close(egl_interface_t* itf)
{
  assert(itf);
  assert(itf->close);

  return itf->close();
}

uint8_t egl_itf_read_byte(egl_interface_t* itf)
{
  uint8_t data = 0;
  size_t  size = sizeof(uint8_t);
  
  assert(egl_itf_read(itf, &data, &size) == EGL_SUCCESS);
  assert(size == sizeof(uint8_t));

  return data;
}

uint16_t egl_itf_read_halfword(egl_interface_t* itf)
{
  uint16_t data = 0;
  size_t   size = sizeof(uint16_t);
  
  assert(egl_itf_read(itf, &data, &size) == EGL_SUCCESS);
  assert(size == sizeof(uint16_t));

  return data;
}

uint32_t egl_itf_read_word(egl_interface_t* itf)
{
  uint32_t data = 0;
  size_t   size = sizeof(uint32_t);
  
  assert(egl_itf_read(itf, &data, &size) == EGL_SUCCESS);
  assert(size == sizeof(uint32_t));

  return data;
}

uint64_t egl_itf_read_longword(egl_interface_t* itf)
{
  uint64_t data = 0;
  size_t   size = sizeof(uint64_t);
  
  assert(egl_itf_read(itf, &data, &size) == EGL_SUCCESS);
  assert(size == sizeof(uint64_t));

  return data;
}

void egl_itf_write_byte(egl_interface_t* itf, uint8_t data)
{
  size_t  size = sizeof(uint8_t);
  
  assert(egl_itf_write(itf, &data, &size) == EGL_SUCCESS);
  assert(size == sizeof(uint8_t));
}

void egl_itf_write_halfword(egl_interface_t* itf, uint16_t data)
{
  size_t  size = sizeof(uint16_t);
  
  assert(egl_itf_write(itf, &data, &size) == EGL_SUCCESS);
  assert(size == sizeof(uint16_t));
}

void egl_itf_write_word(egl_interface_t* itf, uint32_t data)
{
  size_t  size = sizeof(uint32_t);
  
  assert(egl_itf_write(itf, &data, &size) == EGL_SUCCESS);
  assert(size == sizeof(uint32_t));
}

void egl_itf_write_longword(egl_interface_t* itf, uint64_t data)
{
  size_t  size = sizeof(uint64_t);
  
  assert(egl_itf_write(itf, &data, &size) == EGL_SUCCESS);
  assert(size == sizeof(uint64_t));
}
