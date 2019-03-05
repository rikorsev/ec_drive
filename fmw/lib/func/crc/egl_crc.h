#ifndef EGL_CRC_H
#define EGL_CRC_H

typedef struct
{
  uint32_t last_val;
  uint32_t start_val;
  
  void     (*init)(void);
  void     (*poly_set)(uint32_t poly);
  uint8_t  (*calc8)(uint8_t crc, void* data, size_t len);
  uint16_t (*calc16)(uint16_t crc, void* data, size_t len);
  uint32_t (*calc32)(uint32_t crc, void* data, size_t len);
  void     (*deinit)(void);
}egl_crc_t;

void     egl_crc_init    (egl_crc_t *crc, uint32_t poly, uint32_t start_val);
void     egl_crc_poly_set(egl_crc_t *crc, uint32_t poly);
void     egl_crc_reset   (egl_crc_t *crc);
uint8_t  egl_crc8_calc   (egl_crc_t *crc, void* data, size_t len);
uint16_t egl_crc16_calc  (egl_crc_t *crc, void* data, size_t len);
uint32_t egl_crc32_calc  (egl_crc_t *crc, void* data, size_t len);
void     egl_crc_deinit  (egl_crc_t *crc);

#endif
