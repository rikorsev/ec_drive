#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include "egl_crc.h"

void egl_crc_init(egl_crc_t *crc, uint32_t poly, uint32_t start_val)
{
  assert(crc != NULL);

  if(crc->init != NULL)
    {
      crc->init();
    }

  crc->start_val = start_val;
  egl_crc_poly_set(crc, poly);
  egl_crc_reset(crc);
}

void egl_crc_poly_set(egl_crc_t *crc, uint32_t poly)
{
  assert(crc != NULL);

  if(crc->poly_set != NULL)
    {
      crc->poly_set(poly);
    }
}

void egl_crc_reset(egl_crc_t *crc)
{
  crc->last_val = crc->start_val;
}

uint8_t egl_crc8_calc(egl_crc_t *crc, void* data, size_t len)
{
  assert(crc           != NULL);
  assert(crc->calc8    != NULL);
  assert(data          != NULL);

  crc->last_val = crc->calc8(crc->last_val, data, len);
  return (uint8_t)crc->last_val;
}

uint16_t egl_crc16_calc(egl_crc_t *crc, void* data, size_t len)
{
  assert(crc           != NULL);
  assert(crc->calc16   != NULL);
  assert(data          != NULL);

  crc->last_val = crc->calc16(crc->last_val, data, len);
  return (uint16_t)crc->last_val;
}

uint32_t egl_crc32_calc(egl_crc_t *crc, void* data, size_t len)
{
  assert(crc           != NULL);
  assert(crc->calc32   != NULL);
  assert(data          != NULL);

  crc->last_val = crc->calc32(crc->last_val, data, len);
  return (uint32_t)crc->last_val;
}

void egl_crc_deinit(egl_crc_t *crc)
{
  assert(crc           != NULL);

  if(crc->deinit != NULL)
    {
      crc->deinit();
    }
}
