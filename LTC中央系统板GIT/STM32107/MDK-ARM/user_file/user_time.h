#ifndef __USER_TIME_H
#define __USER_TIME_H

#include "stm32f1xx_hal.h"
#include "user_config.h"



extern uint8_t get_can_sent_flag(void);
extern void clr_can_sent_flag(void);
extern void user_time_init(void);

#endif /* __USER_TIME_H */

