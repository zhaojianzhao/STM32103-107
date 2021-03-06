#ifndef __USER_CAN_H
#define __USER_CAN_H
#include "stm32f1xx_hal.h"

extern void user_can_init(void);

extern void time_event(void);          //放在主函数中轮询 做前台工作；

extern uint8_t get_high_speed_date(uint16_t msg_addr,uint8_t motion);
extern uint8_t get_update_flag(void);
#endif
