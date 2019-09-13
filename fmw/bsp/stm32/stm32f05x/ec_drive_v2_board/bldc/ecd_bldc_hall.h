#ifndef ECD_BLDC_HALL_H
#define ECD_BLDC_HALL_H

#define ECD_BLDC_HALL_1_EXTI       (EXTI_Line0)
#define ECD_BLDC_HALL_2_EXTI       (EXTI_Line1)
#define ECD_BLDC_HALL_3_EXTI       (EXTI_Line11)

egl_bldc_hall_t* ecd_bldc_hall(void);

#endif //ECD_BLDC_HALL_H
