#include "stm32f0xx.h"
#include "egl_lib.h"

static void init(void)
{
  /* Enable CRC AHB clock interface */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
}

static uint32_t calc32(uint32_t crc, void* data, size_t len)
{
  /* Init the INIT register */
  CRC_SetInitRegister(crc);
  
  /* Compute the CRC value of the 8-bit buffer: CRCBuffer */
  return (uint32_t)CRC_CalcBlockCRC(data, len);
}

static void deinit(void)
{
  /* DeInit CRC peripheral */
  CRC_DeInit();
}

static egl_crc_t crc_impl =
{
  .init     = init,
  .poly_set = NULL,
  .calc8    = NULL,
  .calc16   = NULL,
  .calc32   = calc32,
  .deinit   = deinit
};

egl_crc_t *crc(void)
{
  return &crc_impl;
}
