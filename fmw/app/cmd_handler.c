#define EGL_MODULE_NAME "CMD"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "egl_lib.h"
#include "ecd_bsp.h"
#include "monitoring.h"
#include "cmd_handler.h"

#define SPI_LLP_BUFF_SIZE (64)

static egl_crc_t crc_in = 
{
  .start_val = 0,
  .calc16    = egl_crc16_xmodem_calc
};

static egl_crc_t crc_out = 
{
  .start_val = 0,
  .calc16    = egl_crc16_xmodem_calc
};

static egl_result_t cmd_motor_start(const void *in, size_t len_in, void *out, size_t *len_out)
{
  egl_result_t result = EGL_FAIL;
  
  EGL_TRACE_INFO("Motor start");
  
  result = egl_bldc_start(motor());
  if(result != EGL_SUCCESS)
  {
    EGL_TRACE_ERROR("Motor start - fail. Result: %s", EGL_RESULT());
  }
  
  return result;
}

static egl_result_t cmd_motor_stop(const void *in, size_t len_in, void *out, size_t *len_out)
{
  egl_result_t result = EGL_FAIL;
  
  EGL_TRACE_INFO("Motor stop");
  
  result = egl_bldc_stop(motor());  
  if(result != EGL_SUCCESS)
  {
    EGL_TRACE_ERROR("Motor stop - fail. Result: %s", EGL_RESULT());
  }
  
  return result;
}

static uint16_t motor_power_convert(uint32_t val)
{
  return val; /*TBD*/
}

static egl_result_t cmd_motor_power_set(const void *in, size_t len_in, void *out, size_t *len_out)
{
  uint32_t speed = *(uint32_t *)in;
  egl_result_t result = EGL_SUCCESS;
  
  EGL_TRACE_INFO("Motor set speed %d", speed);

  result = egl_bldc_set_power(motor(), motor_power_convert(speed));

  return result;
}

static egl_result_t cmd_monitoring_start(const void *in, size_t len_in, void *out, size_t *len_out)
{
  EGL_TRACE_INFO("Monitoring start");

  monitoring_start();

  return EGL_SUCCESS;
}

static egl_result_t cmd_monitoring_stop(const void *in, size_t len_in, void *out, size_t *len_out)
{
  EGL_TRACE_INFO("Monitoring stop");

  monitoring_stop();

  return EGL_SUCCESS;  
}

static egl_result_t cmd_test(const void *in, size_t len_in, void *out, size_t *len_out)
{
  EGL_TRACE_INFO("Test command");

  for(int i = 0; i < len_in; i++)
  {
    EGL_TRACE_INFO("In: 0x%02x", ((uint8_t*)in)[i]);
  }

  memcpy(out, in, len_in);
  *len_out = len_in;

  return EGL_SUCCESS;
}

static egl_result_t cmd_enable_trace(const void *in, size_t len_in, void *out, size_t *len_out)
{
  egl_trace_enable();

  return EGL_SUCCESS;
}

static egl_result_t cmd_disable_trace(const void *in, size_t len_in, void *out, size_t *len_out)
{
  egl_trace_disable();

  return EGL_SUCCESS;
}

static egl_result_t cmd_reset(const void *in, size_t len_in, void *out, size_t *len_out)
{
  egl_result_t result = EGL_FAIL;

  /* Swich to polling mode */
  egl_itf_ioctl(dbg(), DBG_WRITE_POLLING_IOCTL, NULL, 0);

  EGL_TRACE_INFO("Reset");

  /* Stop motor */
  result = egl_bldc_stop(motor());  
  if(result != EGL_SUCCESS)
  {
    EGL_TRACE_ERROR("Motor stop - fail. Result: %s", EGL_RESULT());
    return result;
  }

  /* Reset board */
  result = egl_board_reset(board());
  if(result != EGL_SUCCESS)
  {
    EGL_TRACE_ERROR("Reset - fail. Result: %s", EGL_RESULT());
  }

  return result;
}

static const egl_llp_req_t cmd_map[] =
{
  { .id = CMD_MOTOR_START_ID,      .handler = cmd_motor_start         },
  { .id = CMD_MOTOR_STOP_ID,       .handler = cmd_motor_stop          },
  { .id = CMD_MOTOR_SET_POWER_ID,  .handler = cmd_motor_power_set     },
  { .id = CMD_MONITORING_START_ID, .handler = cmd_monitoring_start    },
  { .id = CMD_MONITORING_STOP_ID,  .handler = cmd_monitoring_stop     },
  { .id = CMD_ENABLE_TRACE_ID,     .handler = cmd_enable_trace        },
  { .id = CMD_DISABLE_TRACE_ID,    .handler = cmd_disable_trace       },
  { .id = CMD_RESET_ID,            .handler = cmd_reset               },
  { .id = CMD_TEST_ID,             .handler = cmd_test                }
};

EGL_LLP_DECLARE(spi_llp_impl, cmd_map, &crc_in, &crc_out, SPI_LLP_BUFF_SIZE, SPI_LLP_BUFF_SIZE);

egl_ptc_t* spi_llp(void)
{
  return &spi_llp_impl;
}

