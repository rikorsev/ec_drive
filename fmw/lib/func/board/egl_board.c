#include <stdlib.h>

#include "egl_result.h"
#include "egl_board.h"

egl_result_t egl_board_reset(egl_board_t *board)
{
	if(board->reset == NULL)
	{
		return EGL_NOT_SUPPORTED;
	}

	return board->reset();
}