#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#include "stm32f1xx_hal.h"

#define BUS_CAN   //ʹ��CAN����ģʽ

#define SEAT_AMOUNT 10 //�������εĸ�����
#define HEART_BEAT 0x200  //������ID�Ŷκţ�
#define SAFE(x) do{ \
	__set_PRIMASK(1); \
	x; \
	__set_PRIMASK(0); \
}while(0)	//ԭ�Ӳ���

/*����Msg������*/
enum MSG
{
	HIGHT_MSG=0x100,  //�߶�ID
	SPEED_MSG,					//�ٶ�ID
	SP_MSG					  //��ЧID	
};	

extern uint8_t send_id;  //���������������־
extern uint16_t StdId_buff[SEAT_AMOUNT];
extern void can_process(void);

extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
#endif
