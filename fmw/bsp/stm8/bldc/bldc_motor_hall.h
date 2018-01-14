#ifndef BLDC_MOTOR_HALL_H
#define BLDC_MOTOR_HALL_H

typedef enum
{
  HALL_STATE_1 = (uint8_t)0x28,
  HALL_STATE_2 = (uint8_t)0x08,
  HALL_STATE_3 = (uint8_t)0x18,
  HALL_STATE_4 = (uint8_t)0x10,
  HALL_STATE_5 = (uint8_t)0x30,
  HALL_STATE_6 = (uint8_t)0x20,
}motor_hall_states_t;

void bldc_hall_init(void);
uint8_t bldc_hall_get(void);

#endif //BLDC_MOTOR_HALL_H
