#ifndef BLDC_MOTOR
#define BLDC_MOTOR

typedef enum
{
  BLDC_MOTOR_READY,
  BLDC_MOTOR_IN_WORK,
  BLDC_MOTOR_HALL_SENSOR_ERROR,
  BLDC_MOTOR_OVERLOAD,
  BLDC_MOTOR_OVERHEAT,
}bldc_state_t;

typedef enum
{
  BLDC_MOTOR_DIR_CW,
  BLDC_MOTOR_DIR_CCW
}bldc_dir_t;

/* typedef bool (*motor_start_func_t)(uint32_t speed, bldc_dir_t dir); */
/* typedef bool (*motor_stop_func_t)(void); */
/* typedef void (*motor_init_func_t)(void); */
/* typedef uint32_t (*motor_get_speed_func_t)(void); */
/* typedef uint32_t (*motor_get_load_func_t)(void); */

/* typedef struct */
/* { */
/*   motor_init_func_t init; */
/*   motor_start_func_t start; */
/*   motor_stop_func_t stop; */
/* }motor_interface_t; */

/* typedef struct */
/* { */
/*   uint32_t speed; */
/*   uint32_t load; */
/*   uint32_t work_time; */
/*   uint32_t work_win1; */
/*   uint32_t work_win2; */
/*   uint32_t work_win3; */
/* }motor_params_t; */

typedef struct
{
  void (*init)(void);
  void (*start)(void);
  void (*stop)(void);
  void (*set)(uint16_t power);
  bool (*switch_wind)(uint8_t hall, bldc_dir_t dir);
}bldc_pwm_t;

typedef struct
{
  void (*init)(void);
  uint8_t (*get)(void);
}bldc_hall_t;

typedef struct
{
  void (*init)(void);
  void (*start)(void);
  void (*stop)(void);
  void (*update)(void);
  uint16_t (*get)(void);
}bldc_speed_meas_t;

typedef struct
{
  void (*init)(void);
  void (*update)(void);
  uint16_t (*get)(void);
}bldc_load_t;

typedef struct
{
  bldc_state_t state;
  bldc_dir_t dir;
  uint16_t power;
  bldc_pwm_t pwm;
  bldc_hall_t hall;
  bldc_speed_meas_t speed;
  bldc_load_t load;
}bldc_t;

void bldc_init(bldc_t *motor);
bool bldc_start(bldc_t *motor); //, uint32_t power, motor_dir_t dir);
bool bldc_stop(bldc_t *motor);
void bldc_set_dir(bldc_t *motor, bldc_dir_t dir);
bldc_dir_t bldc_get_dir(bldc_t *motor);
void bldc_set_power(bldc_t *motor, uint16_t power);
uint32_t bldc_get_speed(bldc_t *motor);
uint16_t bldc_get_load(bldc_t *motor);
bldc_state_t bldc_get_state(bldc_t *motor);
void bldc_hall_handler(bldc_t *motor);
  
//void motor_tension_set_state(motor_state_t state);

#endif //BLDC_MOTOR
