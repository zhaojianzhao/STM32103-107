#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#include "stm32f1xx_hal.h"

#define SEAT_AMOUNT 10 //�������εĸ�����
#define HEART_BEAT 0x200  //������ID�Ŷκţ�

#define SAFE(x) do{ \
	__set_PRIMASK(1); \
	x; \
	__set_PRIMASK(0); \
}while(0)	//ԭ�Ӳ���

enum MSG
{
	HIGHT_MSG=0x100,  //�߶�ID
	SPEED_MSG,					//�ٶ�ID
	SP_MSG					  //��ЧID	
};	


struct status
{
	uint8_t id;					//���α��
};

extern uint8_t send_id;  //���������������־
extern uint16_t StdId_buff[SEAT_AMOUNT];

extern CAN_HandleTypeDef hcan;
extern struct status status;
#endif
