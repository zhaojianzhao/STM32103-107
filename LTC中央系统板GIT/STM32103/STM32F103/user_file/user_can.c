#include "user_can.h"
#include "user_config.h"

CanTxMsgTypeDef TxMessage;
CanRxMsgTypeDef RxMessage;

void Can_TXconfig()
{
  hcan.pTxMsg=&TxMessage;
  hcan.pTxMsg->StdId=0x006;      //设置基本ID；
	hcan.pTxMsg->IDE=CAN_ID_STD;   //设置为标准格式；
	hcan.pTxMsg->RTR=CAN_RTR_DATA; //设置为数据帧；
	hcan.pTxMsg->DLC=8;            //设置数据长度为8个字节；
  hcan.pTxMsg->Data[0]='C';
	hcan.pTxMsg->Data[1]='A';
	hcan.pTxMsg->Data[2]='N';
	hcan.pTxMsg->Data[3]=' ';
	hcan.pTxMsg->Data[4]='I';
	hcan.pTxMsg->Data[5]='S';
	hcan.pTxMsg->Data[6]='O';
	hcan.pTxMsg->Data[7]='K';
}

void Can_RXconfig()
{
   CAN_FilterConfTypeDef sFilterConfig;
	 hcan.pRxMsg = &RxMessage;
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
	
   HAL_CAN_ConfigFilter(&hcan,&sFilterConfig);	
}	


void user_can_init(void)
{ Can_TXconfig();
	Can_RXconfig();
 	HAL_CAN_Receive_IT(&hcan,CAN_FIFO0);
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

HAL_StatusTypeDef can_send(uint16_t dest_addr, uint8_t *data, uint16_t len)
{   
	 uint8_t i;
	 hcan.pTxMsg->StdId=dest_addr; /*设置要发送数据的目标地址*/
	 hcan.pTxMsg->DLC=len;
	 for(i=0;i<len;i++)
	{	 
		 hcan.pTxMsg->Data[i]=data[i];
		  if(HAL_CAN_Transmit(&hcan, 20)==HAL_OK)
		 {
				
		 } 
		 else     
		 { 
			 
			
		 }	
	 } 
	 return HAL_OK;
}

uint16_t StdId_buff[seat_amount];

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{   
		  /*分析接收到的是什么数据*/
	    if(hcan->pRxMsg->StdId==status.id)  /*接收到了对应的数据*/
			{
					/*返回数据帧给上位机*/
				can_send(status.id, hcan->pTxMsg->Data,8); 
			}	
     	HAL_CAN_Receive_IT(hcan,CAN_FIFO0);

}

