#define EGL_MODULE_NAME "CMD"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "egl_lib.h"
#include "ecd_bsp.h"

#define SPI_LLP_BUFF_SIZE (64)

static egl_result_t motor_start(const void *in, size_t len_in, void *out, size_t *len_out)
{
  egl_result_t result = EGL_SUCCESS;
  
  EGL_TRACE_INFO("Motor start\r\n");
  
  result = egl_bldc_start(ecd_bldc_motor());
  if(result != EGL_SUCCESS)
    {
      EGL_TRACE_INFO("Motor start - fail\r\n");
    }
  
  return result;
}

static egl_result_t motor_stop(const void *in, size_t len_in, void *out, size_t *len_out)
{
  egl_result_t result = EGL_SUCCESS;
  
  EGL_TRACE_INFO("Motor stop\r\n");
  
  result = egl_bldc_stop(ecd_bldc_motor());  
  if(result != EGL_SUCCESS)
    {
      EGL_TRACE_INFO("Motor stop - fail\r\n");
    }
  
  return result;
}

static uint16_t motor_power_convert(uint32_t val)
{
  return 0; /*TBD*/
}

static egl_result_t motor_power_set(const void *in, size_t len_in, void *out, size_t *len_out)
{
  uint32_t speed = *(uint32_t *)in;
  egl_result_t result = EGL_SUCCESS;
  
  EGL_TRACE_INFO("Motor set speed %d\r\n", speed);

  result = egl_bldc_set_power(ecd_bldc_motor(), motor_power_convert(speed));

  return result;
}

static egl_result_t test_command(const void *in, size_t len_in, void *out, size_t *len_out)
{

  EGL_TRACE_INFO("Test command\r\n");

  for(int i = 0; i < len_in; i++)
    {
      EGL_TRACE_INFO("In: 0x%02x\r\n", ((uint8_t*)in)[i]);
    }

  memcpy(out, in, len_in);
  *len_out = len_in;

  return EGL_SUCCESS;
}

static const egl_llp_req_t cmd_map[] =
  {
    { 0xC001, motor_start     },
    { 0xC002, motor_stop      },
    { 0xC003, motor_power_set },
    { 0xC0FF, test_command    }
  };

EGL_LLP_DECLARE(spi_llp_impl, cmd_map, egl_crc16_xmodem, SPI_LLP_BUFF_SIZE, SPI_LLP_BUFF_SIZE);

egl_ptc_t* spi_llp(void)
{
  return &spi_llp_impl;
}

