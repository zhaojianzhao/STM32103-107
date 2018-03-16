#include "user_can.h"
#include "user_config.h"
#include "user_time.h"
#include <string.h>

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
static void can_scale32_idmask(void)  
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

/*2秒一次心跳发送轮询模块*/
void heart_beat_checkout(void)
{
	uint8_t i;
	if(get_heart_beat_flag())
	{
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
		clr_heart_beat_flag(); 
		for(i=0;i<SEAT_AMOUNT;i++)
		{
			stdid_buff[i]=0; 
		}
		can_hb_process(); 
	}	
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
	if(msg_id>=0x7ff)
	{
		msg_id=0x7ff;
	}	
	if(msg_id<=0)
	{
		msg_id=0;
	}	
	if(len>=8)
	{
		len=8;
	}	
	if(len<=0)
	{
		len=0;
	}	
	hcan1.pTxMsg->StdId=msg_id; /*设置要发送数据的目标地址*/
	hcan1.pTxMsg->Data[0]=data[0];
	hcan1.pTxMsg->Data[1]=data[1];
	hcan1.pTxMsg->Data[2]=data[2];
	hcan1.pTxMsg->Data[3]=data[3];
	hcan1.pTxMsg->Data[4]=data[4];
	hcan1.pTxMsg->Data[5]=data[5];
	hcan1.pTxMsg->Data[6]=data[6];
	hcan1.pTxMsg->Data[7]=data[7];
	hcan1.pTxMsg->DLC=len;
	if(HAL_CAN_Transmit(&hcan1, 10)!=HAL_OK)
	{
		; /* do nothing */
	} 
	CAN1->IER|=(1<<1);   //防止断开，IER^2位比复位；
}

/*发送SEAT_AMOUNT 次的轮询“心跳”信号给座椅，通过can_send_buff[0]代表座椅地址  can_send_buff[1]代表心跳信号  can_send_buff[2]代表验证码*/
static uint8_t can_send_buff[8]={0x00,0x00,0x55};
void can_hb_process()
{
	for(can_send_buff[0]=0;can_send_buff[0]<SEAT_AMOUNT;can_send_buff[0]++)   //循环发 SEAT_AMOUNT 次数据出去；
	{  
		can_send(HEART_BEAT,can_send_buff,8);	
	}
}

uint16_t stdid_buff[SEAT_AMOUNT];

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{  
	if(hcan->Instance==CAN1)
	{	
		/*如果检测到的STDID号是#define HEART_BEAT 0x200  心跳的ID号；*/
		if(((hcan1.pRxMsg->StdId&HEART_BEAT)==HEART_BEAT)&&(hcan1.pRxMsg->Data[0]==0x01)&&(hcan1.pRxMsg->Data[1]==0x55))
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
/*只做模块的编写，没有实际的去调用跟发送*/
buscan_control_pack_t pack ;

void buscan_control(uint8_t *high, uint8_t sp_seat, uint8_t sp_env,uint8_t *speed, uint8_t seat_id)
{
	memcpy(pack.high,high,sizeof(pack.high));
	memcpy(pack.speed,speed,sizeof(pack.speed));
	pack.sp_seat_env_id[0]=sp_env;
	pack.sp_seat_env_id[1]=sp_seat;
	pack.sp_seat_env_id[2]=seat_id;
	can_send(HIGHT_MSG_ID,pack.high,8);    //先发	HIGHT_MSG_ID=0x100,  //高度ID
	can_send(SPEED_MSG_ID,pack.speed, 8)	;			//SPEED_MSG_ID  速度ID
	can_send(SP_MSG_ID,pack.sp_seat_env_id,8);     //SP_MSG_ID				  //特效ID
}
#endif



uint8_t shared_memory_ram[1024] = {0};
ram_t *ram = (ram_t *)shared_memory_ram;
/*50毫秒一次的动作数据发送*/
void can_action_date_sent(void)
{
	if(get_can_sent_flag())
	{
		clr_can_sent_flag();
		buscan_control(ram->high,ram->sp_seat,ram->sp_env,ram->speed,0);
	}				
}	
