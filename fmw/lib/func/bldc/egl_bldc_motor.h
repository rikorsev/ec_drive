#ifndef EGL_BLDC_MOTOR
#define EGL_BLDC_MOTOR

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
  EGL_BLDC_MOTOR_READY,
  EGL_BLDC_MOTOR_IN_WORK,
  EGL_BLDC_MOTOR_ERROR,
  EGL_BLDC_MOTOR_OVERLOAD,
  EGL_BLDC_MOTOR_OVERHEAT,
}egl_bldc_state_t;

typedef enum
{
  EGL_BLDC_HALL_STATE_1,
  EGL_BLDC_HALL_STATE_2,
  EGL_BLDC_HALL_STATE_3,
  EGL_BLDC_HALL_STATE_4,
  EGL_BLDC_HALL_STATE_5,
  EGL_BLDC_HALL_STATE_6,
  EGL_BLDC_HALL_STATE_ERROR,
}egl_bldc_hall_state_t;

typedef enum
{
  EGL_BLDC_MOTOR_DIR_CW,
  EGL_BLDC_MOTOR_DIR_CCW
}egl_bldc_dir_t;

typedef struct
{
  void         (*init)        (void);
  egl_result_t (*start)       (void);
  egl_result_t (*stop)        (void);
  egl_result_t (*set)         (uint16_t power);
  egl_result_t (*switch_wind) (egl_bldc_hall_state_t hall, egl_bldc_dir_t dir);
  void         (*deinit)      (void);
}egl_bldc_pwm_t;

typedef struct
{
  void                  (*init)   (void);
  egl_bldc_hall_state_t (*get)    (void);
  void                  (*deinit) (void);
}egl_bldc_hall_t;

typedef struct
{
  void      (*init)   (void);
  void      (*start)  (void);
  void      (*stop)   (void);
  void      (*update) (void);
  uint16_t  (*get)    (void);
  void      (*deinit) (void);
}egl_bldc_speed_meas_t;

typedef struct
{
  void      (*init)   (void);
  void      (*start)  (void);
  void      (*stop)   (void);
  void      (*update) (void);
  int16_t   (*get)    (void);
  void      (*deinit) (void);
}egl_bldc_load_t;

typedef struct
{
  egl_bldc_state_t       state;
  egl_bldc_dir_t         dir;
  uint16_t               power;
  egl_bldc_pwm_t*        pwm;
  egl_bldc_hall_t*       hall;
  egl_bldc_speed_meas_t* speed;
  egl_bldc_load_t*       load;
}egl_bldc_t;

void              egl_bldc_init          (egl_bldc_t *motor);
egl_result_t      egl_bldc_start         (egl_bldc_t *motor);
egl_result_t      egl_bldc_stop          (egl_bldc_t *motor);
void              egl_bldc_set_dir       (egl_bldc_t *motor, egl_bldc_dir_t dir);
egl_bldc_dir_t    egl_bldc_get_dir       (egl_bldc_t *motor);
egl_result_t      egl_bldc_set_power     (egl_bldc_t *motor, uint16_t power);
uint32_t          egl_bldc_get_speed     (egl_bldc_t *motor);
int16_t           egl_bldc_get_load      (egl_bldc_t *motor);
egl_bldc_state_t  egl_bldc_get_state     (egl_bldc_t *motor);
void              egl_bldc_hall_handler  (egl_bldc_t *motor);
  
#endif //EGL_BLDC_MOTOR
