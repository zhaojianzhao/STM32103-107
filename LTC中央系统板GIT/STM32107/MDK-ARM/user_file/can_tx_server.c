#include "stm32f1xx_hal.h"
#include "user_can.h"
#include "user_uart.h"
#include "string.h"
#include "user_config.h"


typedef void(*p_fun_t)(void);      //���庯��ָ���������
void can_tx_handle(void);
void can_rx_handle(void);	
void hb_handle(void);
/***************������ʱ���¼������ĺ���***************/
void task_100ms_prf(void)
{
	printf_debug_info();
}	

void task_can_tx(void)
{
	CAN1->IER|=(1<<1); //ȷ��CAN���������Ȳ�Σ�
	can_tx_handle();
	can_rx_handle(); 
	hb_handle();
}	
/***************����CAN����������***************/
static void can_tx_hb(void)    //�������ڲ����ͺ�����
{
	can_hb_process(); 
}	
/***************����CAN�����Ҫ��***************/
typedef struct
{
	uint16_t msg_id;
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

can_tx_msg_data_t can_buff[CAN_TX_MAX_NUM]={0};     //���巢�͵Ľṹ�����飻

task_can_item_t task_can_table[CAN_TX_MAX_NUM]={0};  //����task_can_item_t �ṹ�����飻
/****************ӳ��CAN���͵�ID��*************************/
can_msg_id_t can_msg_tx_id_maping[CAN_TX_MAX_NUM]=
{
	HEART_BEAT_ID
};
/**********************CAN�ⲿ�������ݵĵ��ú���******************************/
void set_heart_msg(uint8_t *tx_data)
{
	memcpy(can_buff[HEART_BEAT_ID_MSG].data, tx_data, 8);
}

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

/****************��ʼ��CAN�ķ��ͽṹ��*************************/
void can_tx_server_init(void)
{
	can_tx_msg_t index;
	for(index = HEART_BEAT_ID_MSG; index < CAN_TX_MAX_NUM; index++)
	{
		task_can_table[index].msg_id=can_msg_tx_id_maping[index];
		task_can_table[index].cycle=tx_msg_cycle_init[index];
		task_can_table[index].count=0;
		task_can_table[index].can_tx=can_tx_fun_init[index];
	}
}

/****************CAN�ķ��ͽ��̴���*************************/
void can_tx_handle(void)
{
	can_tx_msg_t index;
	for(index = HEART_BEAT_ID_MSG; index < CAN_TX_MAX_NUM; index++)
	{
		if(task_can_table[index].count==0)
		{
			task_can_table[index].count=task_can_table[index].cycle;
			task_can_table[index].can_tx();
		}	
		if(task_can_table[index].count>0)
		{
			task_can_table[index].count--;
		}	
	}	
}	


/****************CAN����ģ��Ķ���*************************/
/****************CAN����ģ��Ķ���*************************/
/****************CAN����ģ��Ķ���*************************/
/****************CAN����ģ��Ķ���*************************/
/*������������������������������������������������������������*/

typedef enum
{
	NM_MSG = 0,
	STATUS_MSG,
	CAN_RX_MAX_NUM
}can_rx_msg_t;

/****************���������ṹ�嶨��,	�ع�,��������*************************/
typedef struct
{
	uint8_t flag;
	uint16_t time_out;
	uint16_t hb_seat_id;   //���������DEBUY ��������ID��ʾ�õģ�
}hb_item_t;

hb_item_t hb_item_table[SEAT_AMOUNT]={0};

void hb_item_init(void)
{
	uint8_t index;
	for(index=0;index< SEAT_AMOUNT;index++)
	{
		hb_item_table[index].flag =0;
		hb_item_table[index].time_out=0;  //����Ϊ5���Ľ���ʱ�䣻
		hb_item_table[index].hb_seat_id=0;
	}
}	

void hb_handle(void)
{
	uint8_t index;
	for(index=0;index< SEAT_AMOUNT;index++)
	{
		if(hb_item_table[index].flag==1)
		{
			hb_item_table[index].flag=0;
			hb_item_table[index].time_out=0;
		}
		else
		{
			hb_item_table[index].time_out++;		
		}	
		if(hb_item_table[index].time_out>=1000)     //��ʾ����ĳ�����μ�ⲻ���Ĵ���
		{
			hb_item_table[index].hb_seat_id=0;     
		}	
	}	
}	

uint16_t get_hb_display(uint8_t index)
{
	uint16_t hb_temp;
	hb_temp=hb_item_table[index].hb_seat_id; 
	return hb_temp;
}
/****************CAN���սṹ��Ķ���*************************/
typedef struct
{
	uint8_t data[8];
}can_rx_msg_data_t;	

can_rx_msg_data_t can_rx_buff[CAN_RX_MAX_NUM]={0};
	
typedef struct
{
	uint16_t msg_id;        //���յ�ID��
	uint8_t flag;           //��λ��־��
	p_fun_t can_rx;        //����ִ�г���
	uint16_t count;        //ʱ�������
	uint16_t timeout;     //��ʱʱ�䣻
	p_fun_t timeout_process;     //��ʱ���̴���
}can_rx_item_t;

can_rx_item_t can_rx_table[CAN_RX_MAX_NUM]={0};

/****************CAN����ID����ӳ��*************************/
can_msg_id_t can_msg_rx_id_maping[CAN_RX_MAX_NUM]=
{
	NM_MSG_ID ,
	STATUS_MSG_ID ,
};

/****************CAN���ձ�־λ����*************************/
uint8_t can_rx_flag_init[CAN_RX_MAX_NUM]={0};

/****************CAN��������ִ�к�������*************************/
static void can_rx_nm(void)
{
	 HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
}	
static void can_rx_status(void)
{
	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_14);
}	

p_fun_t can_rx_fun_init[CAN_RX_MAX_NUM]=
{
	&can_rx_nm,
	&can_rx_status,
};	

/****************CAN���ռ�������*************************/
uint16_t can_count_init[CAN_RX_MAX_NUM]={0};

/****************CAN���ճ�ʱ����*************************/
uint16_t can_timeout_init[CAN_RX_MAX_NUM]=
{
	250,   /* 0 NM_MSG:2500ms */ 
	50,    //1 STATUS :500ms ;
};

/****************CAN���ճ�ʱִ�к�������*************************/
static void can_rx_nm_ot(void)
{

}	
static void can_rx_status_ot(void)
{

}	

p_fun_t can_rx_timout_init[CAN_RX_MAX_NUM]=
{
	&can_rx_nm_ot,
	&can_rx_status_ot,
};	

/****************��ʼ��CAN�Ľ��սṹ��*************************/
void can_rx_server_init(void)
{
	can_rx_msg_t index;
	for(index = NM_MSG;index < CAN_RX_MAX_NUM;index++)
	{
		can_rx_table[index].count=0;
		can_rx_table[index].flag=0;
		can_rx_table[index].msg_id=can_msg_rx_id_maping[index];
		can_rx_table[index].can_rx=can_rx_fun_init[index];
		can_rx_table[index].timeout=can_timeout_init[index];
		can_rx_table[index].timeout_process=can_rx_timout_init[index];
	}
}

/****************CAN�Ľ��ս��̴���*************************/
void can_rx_handle(void)
{
	can_rx_msg_t index;
	for(index = NM_MSG;index < CAN_RX_MAX_NUM;index++)
	{
		if(can_rx_table[index].flag==1)
		{
			can_rx_table[index].can_rx();
			can_rx_table[index].flag=0;
			can_rx_table[index].count=0;			
			/*��������*/
			memcpy(can_rx_buff[index].data,hcan1.pRxMsg->Data,8);
		}	
		if(can_rx_table[index].flag!=1)
		{
			can_rx_table[index].count++;
		}	
		/*���г�ʱ����*/
		if(can_rx_table[index].count>=can_rx_table[index].timeout)
		{
			can_rx_table[index].timeout_process();
		}
	}	
}

/****************�ṩ���ⲿ��FLAG��־λ����*************************/
void set_can_rx_flag(uint16_t msg_id)
{
	can_rx_msg_t index;
	/*�����⵽��STDID����#define HEART_BEAT 0x200  ������ID�ţ�*/
	if(((msg_id&HEART_BEAT)==HEART_BEAT)&&(hcan1.pRxMsg->Data[1]==0x01)&&(hcan1.pRxMsg->Data[2]==0x55))
	{
		hb_item_table[msg_id-HEART_BEAT].flag=1;
		hb_item_table[msg_id-HEART_BEAT].hb_seat_id=msg_id;
	}	
	for(index = NM_MSG;index < CAN_RX_MAX_NUM;index++)
	{
		if(msg_id==can_msg_rx_id_maping[index])
		{
			SAFE(can_rx_table[index].flag=1);     //ʹ�ܶ�Ӧ��FLAG��־λ��
		}	
	}
}
