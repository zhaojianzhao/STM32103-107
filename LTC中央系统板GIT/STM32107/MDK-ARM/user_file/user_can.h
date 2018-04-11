#ifndef __USER_CAN_H
#define __USER_CAN_H
#include "stm32f1xx_hal.h"

#define BUS_CAN   //ʹ��CAN����ģʽ

#define SEAT_AMOUNT 10 //�������εĸ�����
#define HEART_BEAT  0x200
typedef enum 
{
	STATUS_MSG_ID = 0x0,
	HIGHT_MSG_ID=0x100,  //�߶�ID
	SPEED_MSG_ID,					//�ٶ�ID
	SP_MSG_ID,					  //��ЧID	
	HEART_BEAT_ID=0x200,     //������ID�Ŷκţ�
	NM_MSG_ID = 0x400
}can_msg_id_t;	

extern void can_hb_process(void);
extern void user_can_init(void);  
extern void can_action_date_sent(void);
extern void time_event(void);
extern void set_can_rx_flag(uint16_t);
extern uint16_t get_hb_display(uint8_t);

#endif
