#ifndef __USER_CAN_H
#define __USER_CAN_H
#include "stm32f1xx_hal.h"

#define BUS_CAN   //使用CAN总线模式

#define SEAT_AMOUNT 10 //设置座椅的个数；
#define HEART_BEAT  0x200
typedef enum 
{
	STATUS_MSG_ID = 0x0,
	HIGHT_MSG_ID=0x100,  //高度ID
	SPEED_MSG_ID,					//速度ID
	SP_MSG_ID,					  //特效ID	
	HEART_BEAT_ID=0x200,     //心跳的ID号段号；
	NM_MSG_ID = 0x400
}can_msg_id_t;	

extern void can_hb_process(void);
extern void user_can_init(void);  
extern void can_action_date_sent(void);
extern void time_event(void);
extern void set_can_rx_flag(uint16_t);
extern uint16_t get_hb_display(uint8_t);

#endif
