#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#include "stm32f1xx_hal.h"

#define seat_amount 10 //�������εĸ�����


#define SAFE(x) do{ \
	__set_PRIMASK(1); \
	x; \
	__set_PRIMASK(0); \
}while(0)	//ԭ�Ӳ���

struct status
{
	uint8_t id;					//���α��
};

extern uint8_t send_id;  //���������������־
extern uint16_t StdId_buff[seat_amount];

extern CAN_HandleTypeDef hcan;
extern struct status status;
#endif
