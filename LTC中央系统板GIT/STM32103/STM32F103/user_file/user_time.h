#ifndef __USER_TIME_H
#define __USER_TIME_H

#include "stm32f1xx_hal.h"
#include "user_config.h"


extern void user_time_init(void);
extern uint8_t get_tick_flag(void);
extern void clr_tick_flag(void);
#endif /* __USER_TIME_H */

