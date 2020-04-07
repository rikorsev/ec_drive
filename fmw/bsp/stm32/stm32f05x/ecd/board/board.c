//#include "core_cm0.h"
#include "stm32f0xx.h"
#include "egl_lib.h"

static egl_result_t reset(void)
{
	NVIC_SystemReset();

	return EGL_SUCCESS;
}

static const egl_board_t board_impl = 
{
	.reset = reset
};

egl_board_t *board(void)
{
	return (egl_board_t *) &board_impl;
}