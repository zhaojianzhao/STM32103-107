#include "user_can.h"
#include "user_config.h"
#include "user_time.h"
#include "user_uart.h"
CanTxMsgTypeDef txmessage;
CanRxMsgTypeDef rxmessage;
struct rx_buff msg_buff[MSG_ID_COUNT];
static void can_txconfig()
{
  hcan.pTxMsg=&txmessage;
  hcan.pTxMsg->StdId=0x006;       //设置基本ID；
	hcan.pTxMsg->IDE=CAN_ID_STD;    //设置为标准格式；
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
static void can_rxconfig(uint8_t filter_num,uint16_t id_list)
{
	CAN_FilterConfTypeDef sFilterConfig;
	hcan.pRxMsg = &rxmessage;
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
static void can_scale32_idmask(uint8_t filter_num)  
{  
	uint16_t      mask,num,tmp,i;  
  CAN_FilterConfTypeDef  sFilterConfig;
  uint32_t stdidarray[3]={HIGHT_MSG_ID,SPEED_MSG_ID,SP_MSG_ID};  
	for(i=0;i<3;i++)
	{
		stdidarray[i]=HIGHT_MSG_ID+i;
	}		    
  sFilterConfig.FilterNumber = filter_num;               //使用过滤器 filter_num  
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;     //配置为掩码模式   
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;    //设置为32位宽  
  sFilterConfig.FilterIdHigh =(stdidarray[0]<<5);      //验证码可以设置为stdidarray[]数组中任意一个，这里使用stdidarray[0]作为验证码 
  sFilterConfig.FilterIdLow =0;  
    
  mask =0x7ff;                       //下面开始计算屏蔽码  
  num =sizeof(stdidarray)/sizeof(stdidarray[0]);  
  for(i =0; i<num; i++)       //屏蔽码位stdidarray[]数组中所有成员的同或结果   
  {  
    tmp =stdidarray[i] ^ (~stdidarray[0]); //所有数组成员与第0个成员进行同或操作  
    mask &=tmp;  
  }  
  sFilterConfig.FilterMaskIdHigh =(mask<<5);  
  sFilterConfig.FilterMaskIdLow =0|0x02;       //只接收数据帧 
    
  sFilterConfig.FilterFIFOAssignment = 0;        //设置通过的数据帧进入到FIFO0中  
  sFilterConfig.FilterActivation = ENABLE;  
  sFilterConfig.BankNumber = 14;  
    
  if(HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)  
  {  
    Error_Handler();  
  }  
} 

void user_can_init(void)
{ can_txconfig();
	can_rxconfig(0,HEART_BEAT);
	can_scale32_idmask(1);  
 	HAL_CAN_Receive_IT(&hcan,CAN_FIFO0);
}

/**
  * @brief  Transmits a CAN frame message.
  * @param  dest_addr: pointer to which dest_adrr  
  * @param  *data: Pointer to data buffer  
  * @param  len: Amount of data to be send  ,the rang of  0-8;
  * @retval HAL status
  */

void can_send(uint16_t msg_addr, uint8_t *data, uint16_t len)
{   
	if(msg_addr>=0x7ff)
	{
		msg_addr=0x7ff;
	}	
	if(len>=8)
	{
		len=8;
	}	
	hcan.pTxMsg->StdId=msg_addr;  /*设置要发送数据的目标地址*/
	hcan.pTxMsg->Data[0]=status.id;
	hcan.pTxMsg->Data[1]=data[1];
	hcan.pTxMsg->Data[2]=data[2];
	hcan.pTxMsg->Data[3]=data[3];
	hcan.pTxMsg->Data[4]=data[4];
	hcan.pTxMsg->Data[5]=data[5];
	hcan.pTxMsg->Data[6]=data[6];
	hcan.pTxMsg->Data[7]=data[7];	
	hcan.pTxMsg->DLC=len;
	if(HAL_CAN_Transmit(&hcan, 1)==HAL_OK)
	{
		; /* do nothing */
	} 
	else     
	{ 

		;/*to do*/
	}	
	CAN1->IER|=(1<<1);
}

/*时间事件*/
static uint32_t time50ms;
static uint8_t hb_tx_flag;
static uint8_t can_send_buff[8]={0,0x01,0x55};
void time_event(void)
{
	uint8_t i;
	if(get_tick_flag())
	{
		time50ms++;
		clr_tick_flag();
		if(time50ms>=500)     //50MS信号灯闪烁；
		{
			time50ms=0;
			HAL_GPIO_TogglePin(OUTPUT_SEATLED5_GPIO_Port,OUTPUT_SEATLED5_Pin);
		}				
	}
	if(hb_tx_flag)
	{
		/*返回"心跳"数据帧给串口板*/
		hb_tx_flag=0;
		can_send((HEART_BEAT+status.id),can_send_buff,8); 
	}	
	if(status.hight_id)
	{
		status.rx_cnt++;
		status.hight_id=0;
		for(i=0;i<8;i++)
		{
			msg_buff[HIGHT_MSG_ID].date[i]=hcan.pRxMsg->Data[i];
		}
	}
	if(status.speed_id)
	{
		status.rx_cnt++;
		status.speed_id=0;
		for(i=0;i<8;i++)
		{
			msg_buff[SPEED_MSG_ID].date[i]=hcan.pRxMsg->Data[i];
		}		
	}
	if(status.sp_id)
	{
		status.rx_cnt++;
		status.sp_id=0;
		for(i=0;i<8;i++)
		{
			msg_buff[SP_MSG_ID].date[i]=hcan.pRxMsg->Data[i];
		}	
	}	
	if(status.rx_cnt==3)
	{
		frame.enable = 1;
    status.rx_cnt=0;		
	}
	CAN1->IER|=(1<<1); //确保CAN可以在线热插拔；
}

uint16_t stdid_buff[SEAT_AMOUNT];
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{   
	/*分析接收到的是什么数据*/
	/*can_send_buff[0]代表座椅地址  can_send_buff[1]代表心跳信号  can_send_buff[2]代表验证码*/
	if(hcan->pRxMsg->StdId==HEART_BEAT)   /*接收到了对应的"心跳"数据*/
	{
		hb_tx_flag=1;
	}	
	if(hcan->pRxMsg->StdId==HIGHT_MSG_ID)		/*收到了动作高度数据0x100段*/
	{
		HAL_GPIO_TogglePin(OUTPUT_SEATLED4_GPIO_Port,OUTPUT_SEATLED4_Pin);
		status.hight_id=1;	
	}	
	if(hcan->pRxMsg->StdId==SPEED_MSG_ID)		/*收到了动作速度数据0x101段*/
	{
		HAL_GPIO_TogglePin(OUTPUT_SEATLED3_GPIO_Port,OUTPUT_SEATLED3_Pin);
		status.speed_id=1;
	}		
	if(hcan->pRxMsg->StdId==SP_MSG_ID)	/*收到了环境特效，座椅特效，ID数据0x102段*/
	{
		HAL_GPIO_TogglePin(OUTPUT_SEATLED3_GPIO_Port,OUTPUT_SEATLED3_Pin);
		status.sp_id=1;
	}		
	CAN1->IER|=(1<<1);
	HAL_CAN_Receive_IT(hcan,CAN_FIFO0);
}



