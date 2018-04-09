#include "user_can.h"
#include "user_config.h"
#include "user_time.h"

CanTxMsgTypeDef txmessage;
CanRxMsgTypeDef rxmessage;
struct rx_buff msg_buff[MSG_ID_COUNT];

static void can_txconfig()
{
  hcan.pTxMsg=&txmessage;
  hcan.pTxMsg->StdId=0x006;      //���û���ID��
	hcan.pTxMsg->IDE=CAN_ID_STD;   //����Ϊ��׼��ʽ��
	hcan.pTxMsg->RTR=CAN_RTR_DATA; //����Ϊ����֡��
	hcan.pTxMsg->DLC=8;            //�������ݳ���Ϊ8���ֽڣ�
  hcan.pTxMsg->Data[0]='C';
	hcan.pTxMsg->Data[1]='A';
	hcan.pTxMsg->Data[2]='N';
	hcan.pTxMsg->Data[3]=' ';
	hcan.pTxMsg->Data[4]='I';
	hcan.pTxMsg->Data[5]='S';
	hcan.pTxMsg->Data[6]='O';
	hcan.pTxMsg->Data[7]='K';
}
/*32λ�б�ģʽ*/
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

/*32λ����ģʽ*/
static void can_scale32_idmask(uint8_t filter_num)  
{  
	uint16_t      mask,num,tmp,i;  
  CAN_FilterConfTypeDef  sFilterConfig;
  uint32_t stdidarray[3]={HIGHT_MSG_ID,SPEED_MSG_ID,SP_MSG_ID};  
	for(i=0;i<3;i++)
	{
		stdidarray[i]=HIGHT_MSG_ID+i;
	}		    
  sFilterConfig.FilterNumber = filter_num;               //ʹ�ù����� filter_num  
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;     //����Ϊ����ģʽ  
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;    //����Ϊ32λ��  
  sFilterConfig.FilterIdHigh =(stdidarray[0]<<5);     //��֤���������Ϊstdidarray[]����������һ��������ʹ��stdidarray[0]��Ϊ��֤��  
  sFilterConfig.FilterIdLow =0;  
    
  mask =0x7ff;                      //���濪ʼ����������  
  num =sizeof(stdidarray)/sizeof(stdidarray[0]);  
  for(i =0; i<num; i++)      //������λstdidarray[]���������г�Ա��ͬ����  
  {  
    tmp =stdidarray[i] ^ (~stdidarray[0]);  //���������Ա���0����Ա����ͬ�����  
    mask &=tmp;  
  }  
  sFilterConfig.FilterMaskIdHigh =(mask<<5);  
  sFilterConfig.FilterMaskIdLow =0|0x02;        //ֻ��������֡  
    
  sFilterConfig.FilterFIFOAssignment = 0;       //����ͨ��������֡���뵽FIFO0��  
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
	hcan.pTxMsg->StdId=msg_addr; /*����Ҫ�������ݵ�Ŀ���ַ*/
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

/*ʱ���¼�*/
static uint32_t time50ms;
static uint8_t hb_tx_flag;
static uint8_t update_flag;
static uint8_t can_send_buff[8]={0,0x01,0x55};
void time_event(void)
{
	uint8_t i;
	if(get_tick_flag())
	{
		time50ms++;
		clr_tick_flag();
		if(time50ms>=500)    //50MS�źŵ���˸��
		{
			time50ms=0;
			HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
		}				
	}
	if(hb_tx_flag)
	{
		/*����"����"����֡�����ڰ�*/
		hb_tx_flag=0;
		can_send((HEART_BEAT+status.id),can_send_buff,8); 
	}	
	if(status.hight_id)
	{
		status.hight_id=0;
		for(i=0;i<8;i++)
		{
			msg_buff[HIGHT_MSG_ID].date[i]=hcan.pRxMsg->Data[i];
		}
	}
	if(status.speed_id)
	{
		status.speed_id=0;
		for(i=0;i<8;i++)
		{
			msg_buff[SPEED_MSG_ID].date[i]=hcan.pRxMsg->Data[i];
		}		
	}
	if(status.sp_id)
	{
		status.sp_id=0;
		for(i=0;i<8;i++)
		{
			msg_buff[SP_MSG_ID].date[i]=hcan.pRxMsg->Data[i];
		}				
	}	
	if(status.rx_cnt==3)
	{
		update_flag = 1;
    status.rx_cnt=0;		
	}	
	CAN1->IER|=(1<<1); //ȷ��CAN���������Ȳ�Σ�
}

uint16_t stdid_buff[SEAT_AMOUNT];
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{   
	/*�������յ�����ʲô����*/
	/*can_send_buff[0]�������ε�ַ  can_send_buff[1]���������ź�  can_send_buff[2]������֤��*/
	if(hcan->pRxMsg->StdId==HEART_BEAT)  /*���յ��˶�Ӧ��"����"����*/
	{
		hb_tx_flag=1;
	}	
	if(hcan->pRxMsg->StdId==HIGHT_MSG_ID)		/*�յ��˶����߶�����0x100��*/
	{
		HAL_GPIO_TogglePin(PE3_GPIO_Port,PE3_Pin);
		status.hight_id=1;	
	}	
	if(hcan->pRxMsg->StdId==SPEED_MSG_ID)		/*�յ��˶����ٶ�����0x101��*/
	{
		HAL_GPIO_TogglePin(PE4_GPIO_Port,PE4_Pin);
		status.speed_id=1;
	}		
	if(hcan->pRxMsg->StdId==SP_MSG_ID)		/*�յ��˻�����Ч��������Ч��ID����0x102��*/
	{
		HAL_GPIO_TogglePin(PE5_GPIO_Port,PE5_Pin);
		status.sp_id=1;
	}		
	CAN1->IER|=(1<<1);
	HAL_CAN_Receive_IT(hcan,CAN_FIFO0);
}

///////////////////////////////*�ṩCAN���ⲿ�ӿ�*////////////////////////////////////////////////
/*��ȡ���β�   ID�κ�  �� �׺�*/

//	HIGHT_MSG_ID=0x100,  //�߶�ID
//	SPEED_MSG_ID,					//�ٶ�ID
//	SP_MSG_ID,					  //��ЧID	    
//	CHAIR_DATA,           //����ID
//	ENV_DATA							//������ЧID
	
uint8_t get_high_speed_date(uint16_t msg_addr,uint8_t motion)
{
	switch(msg_addr)
	{
		case HIGHT_MSG_ID:
				 return msg_buff[HIGHT_MSG_ID].date[motion-1];
		case SPEED_MSG_ID:
				 return msg_buff[SPEED_MSG_ID].date[motion-1];
		case SP_MSG_ID:
				 return msg_buff[SP_MSG_ID].date[1];  //����������Ч
		case CHAIR_DATA:
				 return msg_buff[SP_MSG_ID].date[2];  //��������ID�ţ�		
		case ENV_DATA:
				 return msg_buff[SP_MSG_ID].date[0];  //���ػ�����Ч��
		default:
			   break;
	}	
	return NONE_DATA;
}

/*��ȡCAN���ݵĸ���λ*/
uint8_t get_update_flag(void)
{
	uint8_t update_byte;
	SAFE(update_byte=update_flag);
	return update_byte;
}	

