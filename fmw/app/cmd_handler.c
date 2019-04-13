#define EGL_MODULE_NAME "CMD"

#include <stdint.h>
#include <stdlib.h>

#include "egl_lib.h"

#define SPI_LLP_BUFF_SIZE (64)

static egl_result_t motor_start(void *data, size_t len)
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

static egl_result_t motor_stop(void *data, size_t len)
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

static uint16_t motor_power_convert(uint32_t)
{
  return 0; /*TBD*/
}

static egl_result_t motor_power_set(void *data, size_t len)
{
  uint32_t speed = *(uint32_t *)data;
  egl_result_t result = EGL_SUCCESS;
  
  EGL_TRACE_INFO("Motor set speed %d\r\n", speed);

  result = egl_bldc_set_power(ecd_bldc_motor(), motor_speed_convert(speed));

  return
}

static egl_result_t test_command(void *data, size_t len)
{
  uint32_t test_data = *(uint32_t *)data;
  EGL_TRACE_INFO("Test command. Test data %d (0x%08x)\r\n", test_data);

  return EGL_SUCCESS;
}

static const egl_llp_req_t cmd_map[] =
  {
    { 0xC001, motor_start     },
    { 0xC002, motor_stop      },
    { 0xC003, motor_power_set },
    { 0xC0FF, test_command    }
  };

EGL_LLP_DECLARE(spi_llp_impl, cmd_map, egl_crc16_xmodem, SPI_LLP_BUFF_SIZE);

egl_ptc_t* spi_llp(void)
{
  return &spi_llp_impl;
}
