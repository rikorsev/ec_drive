#ifndef EGL_PIO_H
#define EGL_PIO_H

typedef struct
{
	void         (*init)   (void);
	egl_result_t (*set)    (bool state);
	egl_result_t (*get)    (void);
	egl_result_t (*deinit) (void);
}egl_pio_t;

void           egl_pio_init   (egl_pio_t *pio);
egl_result_t   egl_pio_set    (egl_pio_t *pio, bool state);
egl_result_t   egl_pio_get    (egl_pio_t *pio);
egl_result_t   egl_pio_toggle (egl_pio_t *pio);
egl_result_t   egl_pio_deinit (egl_pio_t *pio);

#endif