#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#include "stm32f1xx_hal.h"

#define SEAT_AMOUNT 10 //设置座椅的个数；
#define NO_ACK 0x666

#define SAFE(x) do{ \
	__set_PRIMASK(1); \
	x; \
	__set_PRIMASK(0); \
}while(0)	//原子操作

extern uint8_t send_id;  //测试连发的数组标志
extern uint16_t StdId_buff[SEAT_AMOUNT];
extern void can_process(void);

extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart3;
#endif
