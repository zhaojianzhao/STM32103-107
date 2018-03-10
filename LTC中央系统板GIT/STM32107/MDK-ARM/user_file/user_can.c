#include "user_can.h"
#include "user_config.h"

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
uint8_t loop;
uint8_t can_send_buff[8]="SHU QEE!";
void can_process()
{
	for(loop=0;loop<SEAT_AMOUNT;loop++)   //循环发十次数据出去；
	{  
		can_send(loop,can_send_buff,8);
		HAL_Delay(2);
		send_id++;
	}
	send_id=0;	
}

uint16_t StdId_buff[SEAT_AMOUNT];

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{   
	if(hcan->Instance==CAN1)
	{
		/*对比现在接收的ID号是否存活*/
		if(hcan1.pRxMsg->StdId==hcan1.pTxMsg->StdId)   /*存活*/
		{  
		StdId_buff[send_id]=hcan1.pRxMsg->StdId;
		}
		else       /*不存活*/
		{
		StdId_buff[send_id]=0x555;   //ID存活的乱码标志0X555;
		}	
		HAL_CAN_Receive_IT(&hcan1,CAN_FIFO0);
	}

}

