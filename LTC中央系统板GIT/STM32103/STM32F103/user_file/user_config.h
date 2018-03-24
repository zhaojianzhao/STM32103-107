#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#include "stm32f1xx_hal.h"

#define SEAT_AMOUNT 10 //设置座椅的个数；
#define HEART_BEAT 0x200  //心跳的ID号段号；

#define SAFE(x) do{ \
	__set_PRIMASK(1); \
	x; \
	__set_PRIMASK(0); \
}while(0)	//原子操作

enum msg
{
	HIGHT_MSG_ID=0x100,  //高度ID
	SPEED_MSG_ID,					//速度ID
	SP_MSG_ID					  //特效ID	
};	


struct status
{
	uint8_t id;					//座椅编号
};

extern uint16_t stdid_buff[SEAT_AMOUNT];

extern CAN_HandleTypeDef hcan;
extern struct status status;
#endif
