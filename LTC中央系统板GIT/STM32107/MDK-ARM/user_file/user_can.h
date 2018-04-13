#ifndef __USER_CAN_H
#define __USER_CAN_H
#include "stm32f1xx_hal.h"

//#define BUS_CAN_DEBUG   //ʹ��CAN����ģʽ

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

extern void user_can_init(void);  
extern void can_action_date_sent(void);
extern void time_event(void);
extern void can_action_handle(void);
extern void set_can_rx_flag(uint16_t);
extern uint16_t get_hb_display(uint8_t);
extern void can_send(uint16_t msg_id, uint8_t *data, uint16_t len);

/**********************�ṩ���ⲿ��������*****************************/
extern void buscan_control(uint8_t *high, uint8_t sp_seat, uint8_t sp_env,uint8_t *speed, uint8_t seat_id);
extern void set_heart_msg(uint8_t *tx_data);
extern void set_hight_msg(uint8_t *tx_data);
extern void set_speed_msg(uint8_t *tx_data);
extern void set_seat_sp_msg(uint8_t *tx_data);

extern void get_status_msg(uint8_t *tx_data);
extern void set_nm_msg(uint8_t *tx_data);

#endif
