#include "user_can.h"
#include "user_config.h"
#include "user_time.h"
#include "user_uart.h"
#include <string.h>

struct status status={0};
CanTxMsgTypeDef txmessage;
CanRxMsgTypeDef rxmessage;

void can_txmsg_config()
{
	hcan1.pTxMsg=&txmessage;
	hcan1.pTxMsg->StdId=0x123;      //设置基本ID；
	hcan1.pTxMsg->IDE=CAN_ID_STD;   //设置为标准格式；
	hcan1.pTxMsg->RTR=CAN_RTR_DATA; //设置为数据帧；
	hcan1.pTxMsg->DLC=8;            //设置数据长度为8个字节；
	hcan1.pTxMsg->Data[0]='C';
	hcan1.pTxMsg->Data[1]='A';
	hcan1.pTxMsg->Data[2]='N';
	hcan1.pTxMsg->Data[3]=' ';
	hcan1.pTxMsg->Data[4]='T';
	hcan1.pTxMsg->Data[5]='E';
	hcan1.pTxMsg->Data[6]='S';
	hcan1.pTxMsg->Data[7]='T';
}

void can_rxmsg_config()
{
	CAN_FilterConfTypeDef sfilterconfig;
	hcan1.pRxMsg = &rxmessage;
	sfilterconfig.BankNumber=14;
	sfilterconfig.FilterActivation=ENABLE;
	sfilterconfig.FilterFIFOAssignment=0;
	sfilterconfig.FilterIdHigh=(HEART_BEAT<<5);
	sfilterconfig.FilterIdLow=0x0000;
	sfilterconfig.FilterMaskIdHigh=(HEART_BEAT<<5);
	sfilterconfig.FilterMaskIdLow=0x0000;
	sfilterconfig.FilterMode=CAN_FILTERMODE_IDMASK;
	sfilterconfig.FilterNumber=0;
	sfilterconfig.FilterScale=CAN_FILTERSCALE_32BIT;

	HAL_CAN_ConfigFilter(&hcan1,&sfilterconfig);	
}	

/*32位宽的掩码模式*/
void can_scale32_idmask(void)  
{  
	uint16_t      mask,num,tmp,i;  
  CAN_FilterConfTypeDef  sfilterconfig;
	hcan1.pRxMsg = &rxmessage;
  uint32_t stdid_array[SEAT_AMOUNT]={0};  
	for(i=0;i<SEAT_AMOUNT;i++)
	{
		stdid_array[i]=HEART_BEAT+i;
	}	
     
  sfilterconfig.FilterNumber = 0;               //使用过滤器0  
  sfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;     //配置为掩码模式  
  sfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;    //设置为32位宽  
  sfilterconfig.FilterIdHigh =(stdid_array[0]<<5);     //验证码可以设置为StdIdArray[]数组中任意一个，这里使用StdIdArray[0]作为验证码  
  sfilterconfig.FilterIdLow =0;  
    
  mask =0x7ff;                      //下面开始计算屏蔽码  
  num =sizeof(stdid_array)/sizeof(stdid_array[0]);  
  for(i =0; i<num; i++)      //屏蔽码位StdIdArray[]数组中所有成员的同或结果  
  {  
    tmp =stdid_array[i] ^ (~stdid_array[0]);  //所有数组成员与第0个成员进行同或操作  
    mask &=tmp;  
  }  
  sfilterconfig.FilterMaskIdHigh =(mask<<5);  
  sfilterconfig.FilterMaskIdLow =0|0x02;        //只接收数据帧  
    
  sfilterconfig.FilterFIFOAssignment = 0;       //设置通过的数据帧进入到FIFO0中  
  sfilterconfig.FilterActivation = ENABLE;  
  sfilterconfig.BankNumber = 14;  
    
  if(HAL_CAN_ConfigFilter(&hcan1, &sfilterconfig) != HAL_OK)  
  {  
    Error_Handler();  
  }  
} 


void user_can_init(void)
{ 
	can_txmsg_config();
	can_rxmsg_config(); 
	HAL_CAN_Receive_IT(&hcan1,CAN_FIFO0);
}

static uint32_t time2s;
/*2秒一次心跳发送轮询模块，并且定义FLAG来标志某个座椅缺减的次数并记录*/
void heart_beat_checkout(void)
{
	uint8_t i;
	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
	for(i=0;i<SEAT_AMOUNT;i++)
	{
		if(stdid_buff[i]==0)        //判断某个座椅是否缺席；
		{
			status.hb_count[i]++;			//缺席次数加一；
			if(status.hb_count[i]>=3)
			{
				stdid_buff[i]=0;         //当缺席的次数大于等于3次就默认缺席；
			}
		}	
		stdid_buff[i]=0;
	}
	can_hb_process(); 
}

/**
  * @brief  Transmits a CAN frame message.
  * @param  dest_addr: pointer to which dest_adrr  
  * @param  *data: Pointer to data buffer  
  * @param  len: Amount of data to be send  ,the rang of  0-8;
  * @retval HAL status
  */

void can_send(uint16_t msg_id, uint8_t *data, uint16_t len)
{  
	uint8_t i;
	if(msg_id>=0x7ff)
	{
		msg_id=0x7ff;
	}	
	if(len>=8)
	{
		len=8;
	}	
	hcan1.pTxMsg->StdId=msg_id; /*设置要发送数据的目标地址*/
	hcan1.pTxMsg->DLC=len;
	for(i=0;i<len;i++)
	{
		hcan1.pTxMsg->Data[i]=data[i];
	}
	if(HAL_CAN_Transmit(&hcan1, 1)!=HAL_OK)
	{
		; /* do nothing */
	} 
	CAN1->IER|=(1<<1);   //防止断开，IER^2位复位；
}

/*发送SEAT_AMOUNT 次的轮询“心跳”信号给座椅，通过can_send_buff[0]代表座椅地址  can_send_buff[1]代表心跳信号  can_send_buff[2]代表验证码*/
static uint8_t can_send_buff[8]={0x00,0x00,0x55};
void can_hb_process()
{
	can_send(HEART_BEAT,can_send_buff,8);	
}

uint16_t stdid_buff[SEAT_AMOUNT];

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{  
	if(hcan->Instance==CAN1)
	{	
		/*如果检测到的STDID号是#define HEART_BEAT 0x200  心跳的ID号；*/
		if(((hcan1.pRxMsg->StdId&HEART_BEAT)==HEART_BEAT)&&(hcan1.pRxMsg->Data[1]==0x01)&&(hcan1.pRxMsg->Data[2]==0x55))
		{
		 stdid_buff[(hcan1.pRxMsg->StdId-HEART_BEAT)]=	hcan1.pRxMsg->StdId;
		}
			
	}
	CAN1->IER|=(1<<1);
	HAL_CAN_Receive_IT(&hcan1,CAN_FIFO0);
}

//////////*以下是模块移植*///////
//////////*以下是模块移植*///////
//////////*以下是模块移植*///////
//////////*以下是模块移植*///////
//////////*以下是模块移植*///////
#pragma pack(1)
#ifndef BUS_CAN

typedef struct
{
	uint8_t high[6];
	uint8_t reserve_0[2];
	uint8_t sp_seat;
	uint8_t sp_env;
	uint8_t reserve_1[6];
	uint8_t reserve_2[255-16];
	uint8_t switch_function;
} ram_t;

typedef struct bus485_control_pack
{	
	uint8_t head;
	uint8_t funcode;
	uint8_t high[3];
	uint8_t sp_seat;
	uint8_t sp_env;
	uint8_t seat_id;
	uint8_t end;
} bus485_control_pack_t;
#else

typedef struct
{
	uint8_t high[6];
	uint8_t reserve_0[2];
	uint8_t sp_seat;
	uint8_t sp_env;
	uint8_t speed[6];
	uint8_t reserve_2[255-16];
	uint8_t switch_function;
} ram_t;

typedef struct buscan_control_pack
{	
	uint8_t high[3];
	uint8_t speed[3];
	uint8_t sp_seat_env_id[3];
} buscan_control_pack_t;
#pragma pack()
#endif

#ifndef BUS_CAN
bus485_control_pack_t pack = {.head=0xff, .end=0xee};

void bus485_control(uint8_t *high, uint8_t sp_seat, uint8_t sp_env, uint8_t seat_id)
{
	pack.funcode = 0xc2;
	memcpy(pack.high, high, sizeof(pack.high));
	pack.sp_seat = sp_seat;
	pack.sp_env = sp_env;
	pack.seat_id = seat_id;
	HAL_UART_Transmit_DMA(&huart3, (uint8_t *)&pack, sizeof(pack));
}

#else
buscan_control_pack_t pack ;
static uint8_t mark_cantx;
void buscan_control(uint8_t *high, uint8_t sp_seat, uint8_t sp_env,uint8_t *speed, uint8_t seat_id)
{
//	memcpy(pack.high,high,sizeof(pack.high));
	memcpy(pack.speed,speed,sizeof(pack.speed));
	pack.sp_seat_env_id[0]=sp_env;
	pack.sp_seat_env_id[1]=sp_seat;
	pack.sp_seat_env_id[2]=seat_id;
	switch(mark_cantx)
	{
		case 0: mark_cantx++;
						can_send(HIGHT_MSG_ID,pack.high,3);    //先发	HIGHT_MSG_ID=0x100,  //高度ID
						break;
		case 1: mark_cantx++;
						can_send(SPEED_MSG_ID,pack.speed, 3)	;			//SPEED_MSG_ID  速度ID
						break;
		case 2: mark_cantx=0;
						clr_can_sent_flag();
						frame.enable=0;
						can_send(SP_MSG_ID,pack.sp_seat_env_id,3);     //SP_MSG_ID				  //特效ID
						break;
	}			
}
#endif

uint8_t shared_memory_ram[1024] = {0};
ram_t *ram = (ram_t *)shared_memory_ram;
/*时间嵌入事件*/
static uint32_t time10s;
static uint8_t update;										//串口数据更新标志
void time_event(void)
{
	SAFE(update=frame.enable);
	if(get_tick_flag())
	{		
		clr_tick_flag(); 
		time2s++;
		time10s++;
		if(time2s>=2000)      //定义2S来发送心跳信号；
		{
			time2s=0;
			heart_beat_checkout();
		}
		if(time10s>=100)      //定义100ms来发送；
		{
			time10s=0;	
			printf_debug_info();
		}	
		
	}	
//	if(get_can_sent_flag())  /*50毫秒一次的动作数据发送*/
//	{
//		buscan_control(ram->high,ram->sp_seat,ram->sp_env,ram->speed,0);
//	}	
	if(update)
	{
		pack.high[0]=frame.buff[2];
		pack.high[1]=frame.buff[3];
		pack.high[2]=frame.buff[4];
		buscan_control(pack.high,frame.buff[6],frame.buff[5],ram->speed,frame.buff[7]);
	}	
	CAN1->IER|=(1<<1); //确保CAN可以在线热插拔；	
}	
