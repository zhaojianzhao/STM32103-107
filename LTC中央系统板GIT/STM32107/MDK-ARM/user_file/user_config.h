#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#include "stm32f1xx_hal.h"

#define SEAT_AMOUNT 10 //设置座椅的个数；
#define HEART_BEAT 0x200  //心跳的ID号；
#define SAFE(x) do{ \
	__set_PRIMASK(1); \
	x; \
	__set_PRIMASK(0); \
}while(0)	//原子操作

/*设置Msg的类型*/
enum MSG
{
	HIGHT_MSG=0x100,  //高度ID
	SP_MSG,					  //特效ID
	SPEED_MSG					//速度ID
};	

//extern uint8_t send_id;  //测试连发的数组标志
extern uint16_t StdId_buff[SEAT_AMOUNT];
extern  uint8_t timer4_enable_heart_beat_flag;
extern void can_process(void);

extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart3;
#endif
