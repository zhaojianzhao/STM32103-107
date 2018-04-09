#include "stm32f1xx_hal.h"
#include "user_can.h"
#include "user_uart.h"

typedef void(*p_fun_t)(void);      //���庯��ָ���������
void can_tx_handle(void);
/***************������ʱ���¼������ĺ���***************/
void task_100ms_prf(void)
{
	printf_debug_info();
}	

void task_can_tx(void)
{
	can_tx_handle();
}	
/***************����CAN����������***************/
static void can_tx_hb(void)    //�������ڲ���������
{
	heart_beat_checkout(); 
}	
/***************����CAN�����Ҫ��***************/
typedef struct
{
	uint8_t msg_id;
	p_fun_t can_tx;  
	uint8_t count;
	uint8_t cycle;
}task_can_item_t;	

typedef struct
{
	uint8_t data[8];
}can_tx_msg_data_t;

typedef enum
{
  HEART_BEAT_ID_MSG=0,
	CAN_TX_MAX_NUM	
}can_tx_msg_t;

typedef enum
{
	NM_MSG = 0,
	STATUS_MSG,
}can_rx_msg_t;

can_tx_msg_data_t can_buff[CAN_TX_MAX_NUM]={0};     //���巢�͵Ľṹ�����飻

task_can_item_t task_can_table[CAN_TX_MAX_NUM]={0};  //����task_can_item_t �ṹ�����飻
/****************ӳ��CAN��ID��*************************/
can_msg_id_t can_msg_id_maping[CAN_TX_MAX_NUM]=
{
	HEART_BEAT_ID
};

/****************��ֵCAN�ķ�������*************************/
uint8_t tx_msg_cycle_init[CAN_TX_MAX_NUM]=
{
		200      //2s�������źŷ��ͣ�
};	

/****************��ֵCAN��ָ�뺯��*************************/
p_fun_t can_tx_fun_init[CAN_TX_MAX_NUM]=
{
	&can_tx_hb
};

/****************��ʼ��CAN�Ľṹ��*************************/
void can_tx_server_init(void)
{
	can_tx_msg_t index;
	for(index = HEART_BEAT_ID_MSG; index < CAN_TX_MAX_NUM; index++)
	{
		task_can_table[index].msg_id=can_msg_id_maping[index];
		task_can_table[index].cycle=tx_msg_cycle_init[index];
		task_can_table[index].count=0;
		task_can_table[index].can_tx=can_tx_fun_init[index];
	}
}

/****************CAN�Ľ��̴���*************************/
void can_tx_handle(void)
{
	can_tx_msg_t index;
	for(index = HEART_BEAT_ID_MSG; index < CAN_TX_MAX_NUM; index++)
	{
		if(task_can_table[index].count==0)
		{
			task_can_table[index].count=task_can_table[index].cycle;
			task_can_table[index].can_tx();
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_15);
		}	
		if(task_can_table[index].count>0)
		{
			task_can_table[index].count--;
		}	
	}	
}	
