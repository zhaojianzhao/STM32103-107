#ifndef __USER_TIME_H
#define __USER_TIME_H

#include "stm32f1xx_hal.h"
#include "user_config.h"


extern uint8_t get_timer4_enable_heart_beat_flag(void);
extern uint8_t get_timer2_enable_can_sent_flag(void);
extern void clr_timer4_enable_heart_beat_flag(void);
extern void clr_timer2_enable_can_sent_flag(void);
extern void user_time_init(void);

#endif /* __USER_TIME_H */

