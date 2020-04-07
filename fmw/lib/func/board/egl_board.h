#ifndef EGL_BOARD_H
#define EGL_BOARD_H

typedef struct
{
	egl_result_t (*reset)(void);
}egl_board_t;

egl_result_t egl_board_reset(egl_board_t *board);

#endif