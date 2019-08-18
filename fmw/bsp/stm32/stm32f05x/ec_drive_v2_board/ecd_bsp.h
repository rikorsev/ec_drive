#ifndef ECD_BSP_H
#define ECD_BSP_H

#include "led/ecd_led.h"
#include "main_timer/ecd_main_timer.h"
#include "clock/ecd_clock.h"
#include "runtime/ecd_runtime.h"
#include "dbg_uart/ecd_dbg_uart.h"
#include "bldc/ecd_bldc_hall.h"
#include "bldc/ecd_bldc.h"
#include "man_ctl/ecd_man_ctl.h"
#include "spi/ecd_spi.h"
#include "crc/ecd_crc.h"
#include "int_pin/ecd_int_pin.h"
#include "int2_pin/ecd_int2_pin.h"

void ecd_bsp_init(void);

#endif // ECD_BSP_H
