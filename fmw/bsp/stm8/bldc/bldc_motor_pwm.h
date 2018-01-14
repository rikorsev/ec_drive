#ifndef BLDC_MOTOR_PWM_H
#define BLDC_MOTOR_PWM_H

void bldc_pwm_init(void);
void bldc_pwm_start(void);
void bldc_pwm_stop(void);
void bldc_pwm_set(uint16_t power);
bool bldc_pwm_switch(uint8_t hall_sensors, bldc_dir_t dir);

#endif //BLDC_MOTOR_PWM_H
