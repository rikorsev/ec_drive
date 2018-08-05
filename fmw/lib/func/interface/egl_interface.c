#include <assert.h>
#include "egl_interface.h"

void egl_itf_init(egl_interface_t* itf)
{
  assert(itf);
  assert(itf->init);
  assert(itf->open);
  assert(itf->close);

  itf->init();
}

egl_itf_status_t egl_itf_open(egl_interface_t* itf)
{
  assert(itf);
  assert(itf->open);

  return itf->open();
}

egl_itf_status_t egl_itf_write(egl_interface_t* itf, void* buff, size_t* len)
{
  assert(itf);
  assert(buff);

  if(itf->write == NULL)
    {
      return EGL_ITF_NOT_SUPPORTED;
    }

  *len = itf->write(buff, *len);
  
  return EGL_ITF_SUCCESS;
}

egl_itf_status_t egl_itf_ioctl(egl_interface_t* itf, uint8_t opcode, void* data, size_t len)
{
  assert(itf);

  if(itf->ioctl == NULL)
    {
      return EGL_ITF_NOT_SUPPORTED;
    }

  return itf->ioctl(opcode, data, len);
}

egl_itf_status_t egl_itf_read(egl_interface_t* itf, void* buff, size_t* len)
{
  assert(itf);
  assert(buff);

  if(itf->read == NULL)
    {
      return EGL_ITF_NOT_SUPPORTED;
    }

  *len = itf->read(buff, *len);
  
  return EGL_ITF_SUCCESS;
}

egl_itf_status_t egl_itf_close(egl_interface_t* itf)
{
  assert(itf);
  assert(itf->close);

  return itf->close();
}
