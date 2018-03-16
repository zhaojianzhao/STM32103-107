#include "user_can.h"
#include "user_config.h"

CanTxMsgTypeDef TxMessage;
CanRxMsgTypeDef RxMessage;

static void Can_TXconfig()
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
/*32位列表模式*/
static void Can_RXconfig(uint8_t filter_num,uint16_t id_list)
{
   CAN_FilterConfTypeDef sFilterConfig;
	 hcan.pRxMsg = &RxMessage;
	 sFilterConfig.BankNumber=14;
	 sFilterConfig.FilterActivation=ENABLE;
   sFilterConfig.FilterFIFOAssignment=0;
	 sFilterConfig.FilterIdHigh=(id_list)<<5;
	 sFilterConfig.FilterIdLow=0x0000;
	 sFilterConfig.FilterMaskIdHigh=(id_list)<<5;
	 sFilterConfig.FilterMaskIdLow=0x0000;
	
	 sFilterConfig.FilterMode=CAN_FILTERMODE_IDLIST;
   sFilterConfig.FilterNumber=filter_num;
	 sFilterConfig.FilterScale=CAN_FILTERSCALE_32BIT;
	
   HAL_CAN_ConfigFilter(&hcan,&sFilterConfig);	
}	

/*32位掩码模式*/
static void CANFilterConfig_Scale32_IdMask_StandardIdOnly(uint8_t filter_num)  
{  
	uint16_t      mask,num,tmp,i;  
  CAN_FilterConfTypeDef  sFilterConfig;
  uint32_t StdIdArray[3]={HIGHT_MSG,SPEED_MSG,SP_MSG};  
	for(i=0;i<3;i++)
	{
		StdIdArray[i]=HIGHT_MSG+i;
	}		    
  sFilterConfig.FilterNumber = filter_num;               //使用过滤器 filter_num  
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;     //配置为掩码模式  
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;    //设置为32位宽  
  sFilterConfig.FilterIdHigh =(StdIdArray[0]<<5);     //验证码可以设置为StdIdArray[]数组中任意一个，这里使用StdIdArray[0]作为验证码  
  sFilterConfig.FilterIdLow =0;  
    
  mask =0x7ff;                      //下面开始计算屏蔽码  
  num =sizeof(StdIdArray)/sizeof(StdIdArray[0]);  
  for(i =0; i<num; i++)      //屏蔽码位StdIdArray[]数组中所有成员的同或结果  
  {  
    tmp =StdIdArray[i] ^ (~StdIdArray[0]);  //所有数组成员与第0个成员进行同或操作  
    mask &=tmp;  
  }  
  sFilterConfig.FilterMaskIdHigh =(mask<<5);  
  sFilterConfig.FilterMaskIdLow =0|0x02;        //只接收数据帧  
    
  sFilterConfig.FilterFIFOAssignment = 0;       //设置通过的数据帧进入到FIFO0中  
  sFilterConfig.FilterActivation = ENABLE;  
  sFilterConfig.BankNumber = 14;  
    
  if(HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)  
  {  
    Error_Handler();  
  }  
} 

void user_can_init(void)
{ Can_TXconfig();
	Can_RXconfig(0,HEART_BEAT);
	CANFilterConfig_Scale32_IdMask_StandardIdOnly(1);  
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
	hcan.pTxMsg->StdId=dest_addr; /*设置要发送数据的目标地址*/
	hcan.pTxMsg->Data[0]=data[0];
	hcan.pTxMsg->Data[1]=data[1];
	hcan.pTxMsg->Data[2]=data[2];
	hcan.pTxMsg->Data[3]=data[3];
	hcan.pTxMsg->Data[4]=data[4];
	hcan.pTxMsg->Data[5]=data[5];
	hcan.pTxMsg->Data[6]=data[6];
	hcan.pTxMsg->Data[7]=data[7];	
	hcan.pTxMsg->DLC=len;
	if(HAL_CAN_Transmit(&hcan, 10)==HAL_OK)
	{
		; /* do nothing */
	} 
	else     
	{ 

		;/*to do*/
	}	
	CAN1->IER|=(1<<1);
}
static uint8_t can_send_buff[8]={0x01,0x55};
uint16_t StdId_buff[SEAT_AMOUNT];
uint32_t HIGHT_MSG_rec,SPEED_MSG_rec,SP_MSG_rec;
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{   
	/*分析接收到的是什么数据*/
	if((hcan->pRxMsg->StdId==(HEART_BEAT+status.id))&&(hcan->pRxMsg->Data[0]==0x00)&&(hcan->pRxMsg->Data[1]==0x55))  /*接收到了对应的"心跳"数据*/
	{
		/*返回"心跳"数据帧给串口板*/
		can_send((HEART_BEAT+status.id),can_send_buff,8); 
	}	
	
	if(hcan->pRxMsg->StdId==HIGHT_MSG)		/*收到了动作高度数据0x100段*/
	{
		HAL_GPIO_TogglePin(PE3_GPIO_Port,PE3_Pin);
		HIGHT_MSG_rec++;
	}	
	if(hcan->pRxMsg->StdId==SPEED_MSG)		/*收到了动作速度数据0x101段*/
	{
		HAL_GPIO_TogglePin(PE4_GPIO_Port,PE4_Pin);
		SPEED_MSG_rec++;
	}		
	if(hcan->pRxMsg->StdId==SP_MSG)		/*收到了环境特效，座椅特效，ID数据0x102段*/
	{
		HAL_GPIO_TogglePin(PE5_GPIO_Port,PE5_Pin);
		SP_MSG_rec++;
	}		
	CAN1->IER|=(1<<1);
	HAL_CAN_Receive_IT(hcan,CAN_FIFO0);
}

