#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "egl_result.h"
#include "egl_pio.h"

void egl_pio_init(egl_pio_t *pio)
{
	assert(pio->init != NULL);

	pio->init();
}

egl_result_t egl_pio_set(egl_pio_t *pio, bool state)
{
	if(pio->set == NULL)
	{
		return EGL_NOT_SUPPORTED;
	}

	return pio->set(state);
}

egl_result_t egl_pio_get(egl_pio_t *pio)
{
	if(pio->set == NULL)
	{
		return EGL_NOT_SUPPORTED;
	}

	return pio->get();
}

egl_result_t egl_pio_toggle(egl_pio_t *pio)
{
	egl_result_t state = egl_pio_get(pio);

	if(state == EGL_SET)
	{
		state = egl_pio_set(pio, false);
	}
	else if(state == EGL_RESET)
	{
		state = egl_pio_set(pio, true);
	}

	return state;
}

egl_result_t egl_pio_deinit(egl_pio_t *pio)
{
	if(pio->deinit != NULL)
	{
		return EGL_NOT_SUPPORTED;
	}

	return pio->deinit();
}
