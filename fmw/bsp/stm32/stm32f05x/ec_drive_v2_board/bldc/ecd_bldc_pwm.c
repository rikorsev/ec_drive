#define EGL_MODULE_NAME "BLDC:PWM"

#include "egl_lib.h"

static void init(void)
{
  
}

static bool start(void)
{
  return true;
}

static bool stop(void)
{
  return true;
}

static void set(uint16_t power)
{
  
}

static inline bool switch_wind_cw(uint8_t hall)
{
  bool result = true;
  
  switch(hall)
    {
    case ECD_BLDC_HALL_STATE_1:
      EGL_TRACE_INFO("CW: Stage 1\r\n");
      break;
  
    case ECD_BLDC_HALL_STATE_2:
      EGL_TRACE_INFO("CW: Stage 2\r\n");
      break;

    case ECD_BLDC_HALL_STATE_3:
      EGL_TRACE_INFO("CW: Stage 3\r\n");
      break;

    case ECD_BLDC_HALL_STATE_4:
      EGL_TRACE_INFO("CW: Stage 4\r\n");
      break;
       
    case ECD_BLDC_HALL_STATE_5:
      EGL_TRACE_INFO("CW: Stage 5\r\n");
      break;

    case ECD_BLDC_HALL_STATE_6:
      EGL_TRACE_INFO("CW: Stage 6\r\n");
      break;

    default:
      EGL_TRACE_ERROR("Unknow hall state 0x%x\r\n", hall);
      result = false;
    }

  return result;
}

static inline bool switch_wind_ccw(uint8_t hall)
{
  bool result = true;
  
  switch(hall)
    {
    case ECD_BLDC_HALL_STATE_1:
      EGL_TRACE_INFO("CCW: Stage 1\r\n");
      break;
  
    case ECD_BLDC_HALL_STATE_2:
      EGL_TRACE_INFO("CCW: Stage 2\r\n");
      break;

    case ECD_BLDC_HALL_STATE_3:
      EGL_TRACE_INFO("CCW: Stage 3\r\n");
      break;

    case ECD_BLDC_HALL_STATE_4:
      EGL_TRACE_INFO("CCW: Stage 4\r\n");
      break;
       
    case ECD_BLDC_HALL_STATE_5:
      EGL_TRACE_INFO("CCW: Stage 5\r\n");
      break;

    case ECD_BLDC_HALL_STATE_6:
      EGL_TRACE_INFO("CCW: Stage 6\r\n");
      break;

    default:
      EGL_TRACE_ERROR("Unknow hall state 0x%x\r\n", hall);
      result = false;
    }

  return result;
}

static bool switch_wind(uint8_t hall, egl_bldc_dir_t dir)
{
  bool result = true;
  
  switch(dir)
    {
    case EGL_BLDC_MOTOR_DIR_CW:
      result = switch_wind_cw(hall);
      break;

    case EGL_BLDC_MOTOR_DIR_CCW
      result = switch_wind_ccw(hall);
      break;

    default:
      EGL_TRACE_ERROR("Unknow direction 0x%x\r\n", dir);
      result = EGL_BLDC_MOTOR_HALL_DIR_ERROR;
    }

  return state;
}

static void deinit(void)
{
  
}

egl_bldc_pwm_t ecd_bldc_pwm_impl =
  {
    .init        = init,
    .start       = start,
    .stop        = stop,
    .set         = set,
    .switch_wind = swithc_wind,
    .deinit      = deinit
  };
