#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#include "stm32f1xx_hal.h"

#define BUS_CAN   //使用CAN总线模式

#define SEAT_AMOUNT 10 //设置座椅的个数；
#define HEART_BEAT 0x200  //心跳的ID号段号；
#define SAFE(x) do{ \
	__set_PRIMASK(1); \
	x; \
	__set_PRIMASK(0); \
}while(0)	//原子操作

/*设置Msg的类型*/
enum msg
{
	HIGHT_MSG_ID=0x100,  //高度ID
	SPEED_MSG_ID,					//速度ID
	SP_MSG_ID					  //特效ID	
};	

extern uint8_t send_id;  //测试连发的数组标志
extern uint16_t stdid_buff[SEAT_AMOUNT];
extern void can_hb_process(void);

extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
#endif
