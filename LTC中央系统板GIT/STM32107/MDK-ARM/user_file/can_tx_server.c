#include "stm32f1xx_hal.h"
#include "user_can.h"
#include "user_uart.h"
#include "string.h"
#include "user_config.h"

struct hb_status hb_status;

typedef void(*p_fun_t)(void);      //定义函数指针类别名；
void can_tx_handle(void);
void can_rx_handle(void);	
/***************定义与时间事件关联的函数***************/
void task_100ms_prf(void)
{
	printf_debug_info();
}	

void task_can_tx(void)
{
	can_tx_handle();
	can_rx_handle(); 
}	
/***************定义CAN的任务函数体***************/
static void can_tx_hb(void)    //心跳的内部发送函数；
{
	can_hb_process(); 
}	
/***************定义CAN的相关要素***************/
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

can_tx_msg_data_t can_buff[CAN_TX_MAX_NUM]={0};     //定义发送的结构体数组；

task_can_item_t task_can_table[CAN_TX_MAX_NUM]={0};  //定义task_can_item_t 结构体数组；
/****************映射CAN发送的ID号*************************/
can_msg_id_t can_msg_tx_id_maping[CAN_TX_MAX_NUM]=
{
	HEART_BEAT_ID
};
/**********************CAN外部发送数据的调用函数******************************/
void set_heart_msg(uint8_t *tx_data)
{
	memcpy(can_buff[HEART_BEAT_ID_MSG].data, tx_data, 8);
}

/****************赋值CAN的发送周期*************************/
uint8_t tx_msg_cycle_init[CAN_TX_MAX_NUM]=
{
		200      //2s的心跳信号发送；
};	

/****************赋值CAN的指针函数*************************/
p_fun_t can_tx_fun_init[CAN_TX_MAX_NUM]=
{
	&can_tx_hb
};

/****************初始化CAN的发送结构体*************************/
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

/****************CAN的发送进程处理*************************/
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


/****************CAN接收模块的定义*************************/
/****************CAN接收模块的定义*************************/
/****************CAN接收模块的定义*************************/
/****************CAN接收模块的定义*************************/
/*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/

typedef enum
{
	NM_MSG = 0,
	STATUS_MSG,
	HEART_BEAT_MSG,
	CAN_RX_MAX_NUM
}can_rx_msg_t;
/****************CAN接收结构体的定义*************************/
typedef struct
{
	uint8_t data[8];
}can_rx_msg_data_t;	

can_rx_msg_data_t can_rx_buff[CAN_RX_MAX_NUM]={0};
	
typedef struct
{
	uint16_t msg_id;        //接收的ID号
	uint8_t flag;           //置位标志；
	p_fun_t can_rx;        //正常执行程序；
	uint16_t count;        //时间计数；
	uint16_t timeout;     //超时时间；
	p_fun_t timeout_process;     //超时进程处理；
}can_rx_item_t;

can_rx_item_t can_rx_table[CAN_RX_MAX_NUM]={0};

/****************CAN接收ID的重映射*************************/
can_msg_id_t can_msg_rx_id_maping[CAN_RX_MAX_NUM]=
{
	NM_MSG_ID ,
	STATUS_MSG_ID ,
	HEART_BEAT_ID      //心跳的ID号段号；
};
/****************CAN接收标志位设置*************************/
uint8_t can_rx_flag_init[CAN_RX_MAX_NUM]={0};
/****************CAN接收正常执行函数设置*************************/
static void can_rx_nm(void)
{
	 HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
}	
static void can_rx_status(void)
{
	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_14);
}	
static void can_rx_hb(void)
{
	stdid_buff[(hcan1.pRxMsg->StdId-HEART_BEAT)]=	hcan1.pRxMsg->StdId;
  /*当缺席的次数大于等于5倍的接收时间默认缺席*/
	hb_status.hb_count[(hcan1.pRxMsg->StdId-HEART_BEAT)]++;
	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_15);
}	

p_fun_t can_rx_fun_init[CAN_RX_MAX_NUM]=
{
	&can_rx_nm,
	&can_rx_status,
	&can_rx_hb
};	
/****************CAN接收计数设置*************************/
uint16_t can_count_init[CAN_RX_MAX_NUM]={0};
/****************CAN接收超时设置*************************/
uint16_t can_timeout_init[CAN_RX_MAX_NUM]=
{
	250,   /* 0 NM_MSG:2500ms */ 
	50,    //1 STATUS :500ms ;
	200    // 2 HEART_BIT : 2000ms;
};
/****************CAN接收超时执行函数设置*************************/
static void can_rx_nm_ot(void)
{

}	
static void can_rx_status_ot(void)
{

}	
static void can_rx_hb_ot(void)
{
   ;; //全部座椅没有连通才进入此函数；
}

p_fun_t can_rx_timout_init[CAN_RX_MAX_NUM]=
{
	&can_rx_nm_ot,
	&can_rx_status_ot,
	&can_rx_hb_ot
};	

/****************初始化CAN的接收结构体*************************/
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
/****************CAN的接收进程处理*************************/
void can_rx_handle(void)
{
	can_rx_msg_t index;
	for(index = NM_MSG;index < CAN_RX_MAX_NUM;index++)
	{
		if(can_rx_table[index].flag==1)
		{
			can_rx_table[index].flag=0;
			can_rx_table[index].count=0;
			can_rx_table[index].can_rx();
			/*缓存数据*/
			memcpy(can_rx_buff[index].data,hcan1.pRxMsg->Data,8);
		}	
		if(can_rx_table[index].flag!=1)
		{
			can_rx_table[index].count++;
		}	
		/*进行超时处理*/
		if(can_rx_table[index].count>=can_rx_table[index].timeout)
		{
			can_rx_table[index].timeout_process();
		}
	}	
}

/****************提供给外部的FLAG标志位更改*************************/
void set_can_rx_flag(uint16_t msg_id)
{
	can_rx_msg_t index;
	/*如果检测到的STDID号是#define HEART_BEAT 0x200  心跳的ID号；*/
	if(((msg_id&HEART_BEAT)==HEART_BEAT)&&(hcan1.pRxMsg->Data[1]==0x01)&&(hcan1.pRxMsg->Data[2]==0x55))
	{
		msg_id=msg_id&HEART_BEAT;
	}	
	for(index = NM_MSG;index < CAN_RX_MAX_NUM;index++)
	{
		if(msg_id==can_msg_rx_id_maping[index])
		{
			SAFE(can_rx_table[index].flag=1);     //使能对应的FLAG标志位；
		}	
	}
}
