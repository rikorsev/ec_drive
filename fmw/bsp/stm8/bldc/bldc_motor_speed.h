#ifndef BLDC_MOTOR_SPEED_H
#define BLDC_MOTOR_SPEED_H

void bldc_speed_init(void);
void bldc_speed_start(void);
void bldc_speed_stop(void);
void bldc_speed_update(void);
uint16_t bldc_speed_get(void);

#endif //BLDC_MOTOR_SPEED_H
