#include "user_can.h"
#include "user_config.h"
#include "user_time.h"
#include <string.h>

CanTxMsgTypeDef TxMessage;
CanRxMsgTypeDef RxMessage;

void Can_TXconfig()
{
	hcan1.pTxMsg=&TxMessage;
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

void Can_RXconfig()
{
	CAN_FilterConfTypeDef sFilterConfig;
	hcan1.pRxMsg = &RxMessage;
	sFilterConfig.BankNumber=14;
	sFilterConfig.FilterActivation=ENABLE;
	sFilterConfig.FilterFIFOAssignment=0;
	sFilterConfig.FilterIdHigh=0x0000;
	sFilterConfig.FilterIdLow=0x0000;
	sFilterConfig.FilterMaskIdHigh=0x0000;
	sFilterConfig.FilterMaskIdLow=0x0000;
	sFilterConfig.FilterMode=CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterNumber=0;
	sFilterConfig.FilterScale=CAN_FILTERSCALE_32BIT;

	HAL_CAN_ConfigFilter(&hcan1,&sFilterConfig);	
}	


void user_can_init(void)
{ 
	Can_TXconfig();
	Can_RXconfig();
	HAL_CAN_Receive_IT(&hcan1,CAN_FIFO0);
}


void can_receive_callback(uint16_t sour_addr, uint8_t *data, uint16_t len)
{

}

/*心跳发送轮询模块*/
void heart_beat_checkout(void)
{
	if(timer4_enable_heart_beat_flag)
	{
		timer4_enable_heart_beat_flag=0;
		can_process();
	}	
}

/**
  * @brief  Transmits a CAN frame message.
  * @param  dest_addr: pointer to which dest_adrr  
  * @param  *data: Pointer to data buffer  
  * @param  len: Amount of data to be send  ,the rang of  0-8;
  * @retval HAL status
  */

void can_send(uint16_t dest_addr, uint8_t *data, uint16_t len)
{   
	if(dest_addr>=0x7ff)
	{
		dest_addr=0x7ff;
	}	
	if(dest_addr<=0)
	{
		dest_addr=0;
	}	
	if(len>=8)
	{
		len=8;
	}	
	if(len<=0)
	{
		len=0;
	}	
	hcan1.pTxMsg->StdId=dest_addr; /*设置要发送数据的目标地址*/
	hcan1.pTxMsg->DLC=len;
	if(HAL_CAN_Transmit(&hcan1, 20)==HAL_OK)
	{
		; /* do nothing */
	} 
	else     
	{ 
		; /* TODO: */
	}	
}

/*发送SEAT_AMOUNT 次的轮询“心跳”信号给座椅*/
static uint16_t loop;
static uint8_t can_send_buff[8]={0x00,0x55};
void can_process()
{
	for(loop=0x200;loop<loop+SEAT_AMOUNT;loop++)   //循环发 SEAT_AMOUNT 次数据出去；
	{  
		can_send(loop,can_send_buff,8);
		HAL_Delay(1);
//		send_id++;
	}
//	send_id=0;	
}

uint16_t StdId_buff[SEAT_AMOUNT];

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{   
	if(hcan->Instance==CAN1)
	{
//		/*对比现在接收的ID号是否存活*/
//		if(hcan1.pRxMsg->StdId==hcan1.pTxMsg->StdId)   /*存活*/
//		{  
//		StdId_buff[send_id]=hcan1.pRxMsg->StdId;
//		}
//		else       /*不存活*/
//		{
//		StdId_buff[send_id]=0x555;   //ID存活的乱码标志0X555;
//		}	
		
		/*如果检测到的STDID号是#define HEART_BEAT 0x200  心跳的ID号；*/
		if(hcan1.pRxMsg->StdId==HEART_BEAT)
		{
			
		}
	}
	HAL_CAN_Receive_IT(&hcan1,CAN_FIFO0);
}

//////////*以下是模块移植*///////
#pragma pack(1)

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

#pragma pack()

bus485_control_pack_t pack = {.head=0xff, .end=0xee};

void buscan_control(uint8_t *high, uint8_t sp_seat, uint8_t sp_env, uint8_t seat_id)
{
	pack.funcode = 0xc2;
	memcpy(pack.high, high, sizeof(pack.high));
	pack.sp_seat = sp_seat;
	pack.sp_env = sp_env;
	pack.seat_id = seat_id;
	HAL_UART_Transmit_DMA(&huart3, (uint8_t *)&pack, sizeof(pack));
}

