#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#include "stm32f1xx_hal.h"

#define seat_amount 10 //设置座椅的个数；


#define SAFE(x) do{ \
	__set_PRIMASK(1); \
	x; \
	__set_PRIMASK(0); \
}while(0)	//原子操作

struct status
{
	uint8_t id;					//座椅编号
};

extern uint8_t send_id;  //测试连发的数组标志
extern uint16_t StdId_buff[seat_amount];

extern CAN_HandleTypeDef hcan;
extern struct status status;
#endif
