#ifndef HALL_H
#define HALL_H

#define HALL_1_EXTI       (EXTI_Line0)
#define HALL_2_EXTI       (EXTI_Line1)
#define HALL_3_EXTI       (EXTI_Line11)

egl_bldc_hall_t* hall(void);

#endif //HALL_H
