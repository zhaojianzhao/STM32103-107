#ifndef __USER_CAN_H
#define __USER_CAN_H
#include "stm32f1xx_hal.h"

extern void user_can_init(void);  
extern void heart_beat_checkout(void);
extern void can_send(uint16_t dest_addr, uint8_t *data, uint16_t len);
extern void can_action_date_sent(void);
#endif
