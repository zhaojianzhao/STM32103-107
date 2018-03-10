#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#include "stm32f1xx_hal.h"

#define SEAT_AMOUNT 10 //�������εĸ�����
#define NO_ACK 0x666

#define SAFE(x) do{ \
	__set_PRIMASK(1); \
	x; \
	__set_PRIMASK(0); \
}while(0)	//ԭ�Ӳ���

extern uint8_t send_id;  //���������������־
extern uint16_t StdId_buff[SEAT_AMOUNT];
extern void can_process(void);

extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart3;
#endif
