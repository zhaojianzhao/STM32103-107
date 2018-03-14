#include "user_can.h"
#include "user_config.h"
#include "user_time.h"
#include <string.h>

CanTxMsgTypeDef TxMessage;
CanRxMsgTypeDef RxMessage;

void Can_TXconfig()
{
	hcan1.pTxMsg=&TxMessage;
	hcan1.pTxMsg->StdId=0x123;      //���û���ID��
	hcan1.pTxMsg->IDE=CAN_ID_STD;   //����Ϊ��׼��ʽ��
	hcan1.pTxMsg->RTR=CAN_RTR_DATA; //����Ϊ����֡��
	hcan1.pTxMsg->DLC=8;            //�������ݳ���Ϊ8���ֽڣ�
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

/*32λ�������ģʽ*/
static void CANFilterConfig_Scale32_IdMask_StandardIdOnly(void)  
{  
	uint16_t      mask,num,tmp,i;  
  CAN_FilterConfTypeDef  sFilterConfig;
	hcan1.pRxMsg = &RxMessage;
  uint32_t StdIdArray[SEAT_AMOUNT]={0};  
	for(i=0;i<SEAT_AMOUNT;i++)
	{
		StdIdArray[i]=HEART_BEAT+i;
	}	
     
  sFilterConfig.FilterNumber = 0;               //ʹ�ù�����0  
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;     //����Ϊ����ģʽ  
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;    //����Ϊ32λ��  
  sFilterConfig.FilterIdHigh =(StdIdArray[0]<<5);     //��֤���������ΪStdIdArray[]����������һ��������ʹ��StdIdArray[0]��Ϊ��֤��  
  sFilterConfig.FilterIdLow =0;  
    
  mask =0x7ff;                      //���濪ʼ����������  
  num =sizeof(StdIdArray)/sizeof(StdIdArray[0]);  
  for(i =0; i<num; i++)      //������λStdIdArray[]���������г�Ա��ͬ����  
  {  
    tmp =StdIdArray[i] ^ (~StdIdArray[0]);  //���������Ա���0����Ա����ͬ�����  
    mask &=tmp;  
  }  
  sFilterConfig.FilterMaskIdHigh =(mask<<5);  
  sFilterConfig.FilterMaskIdLow =0|0x02;        //ֻ��������֡  
    
  sFilterConfig.FilterFIFOAssignment = 0;       //����ͨ��������֡���뵽FIFO0��  
  sFilterConfig.FilterActivation = ENABLE;  
  sFilterConfig.BankNumber = 14;  
    
  if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)  
  {  
    Error_Handler();  
  }  
} 



void user_can_init(void)
{ 
	Can_TXconfig();
//	Can_RXconfig();
	CANFilterConfig_Scale32_IdMask_StandardIdOnly(); 
	HAL_CAN_Receive_IT(&hcan1,CAN_FIFO0);
}


void can_receive_callback(uint16_t sour_addr, uint8_t *data, uint16_t len)
{

}

/*2��һ������������ѯģ��*/
void heart_beat_checkout(void)
{
	uint8_t i;
	if(get_timer4_enable_heart_beat_flag())
	{
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
		clr_timer4_enable_heart_beat_flag();
		for(i=0;i<SEAT_AMOUNT;i++)
		{
			StdId_buff[i]=0;
		}
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
	hcan1.pTxMsg->StdId=dest_addr; /*����Ҫ�������ݵ�Ŀ���ַ*/
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
	CAN1->IER|=(1<<1);   //��ֹ�Ͽ���IER^2λ�ȸ�λ��
}

/*����SEAT_AMOUNT �ε���ѯ���������źŸ�����*/
static uint16_t loop=HEART_BEAT;
static uint8_t can_send_buff[8]={0x00,0x55};
void can_process()
{
	for(loop=HEART_BEAT;loop<(HEART_BEAT+SEAT_AMOUNT);loop++)   //ѭ���� SEAT_AMOUNT �����ݳ�ȥ��
	{  
		can_send(loop,can_send_buff,8);	
	}
}

uint16_t StdId_buff[SEAT_AMOUNT];

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{  
	if(hcan->Instance==CAN1)
	{	
		/*�����⵽��STDID����#define HEART_BEAT 0x200  ������ID�ţ�*/
		if(((hcan1.pRxMsg->StdId&HEART_BEAT)==HEART_BEAT)&&(hcan1.pRxMsg->Data[0]==0x01)&&(hcan1.pRxMsg->Data[1]==0x55))
		{
		 StdId_buff[(hcan1.pRxMsg->StdId-HEART_BEAT)]=	hcan1.pRxMsg->StdId;
		}
			
	}
	CAN1->IER|=(1<<1);
	HAL_CAN_Receive_IT(&hcan1,CAN_FIFO0);
}

//////////*������ģ����ֲ*///////
//////////*������ģ����ֲ*///////
//////////*������ģ����ֲ*///////
//////////*������ģ����ֲ*///////
//////////*������ģ����ֲ*///////
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
/*ֻ��ģ��ı�д��û��ʵ�ʵ�ȥ���ø�����*/
buscan_control_pack_t pack ;

void buscan_control(uint8_t *high, uint8_t sp_seat, uint8_t sp_env,uint8_t *speed, uint8_t seat_id)
{
	memcpy(pack.high,high,sizeof(pack.high));
	memcpy(pack.speed,speed,sizeof(pack.speed));
	pack.sp_seat_env_id[0]=sp_env;
	pack.sp_seat_env_id[1]=sp_seat;
	pack.sp_seat_env_id[2]=seat_id;
	can_send(HIGHT_MSG,pack.high,8);    //�ȷ�	HIGHT_MSG=0x100,  //�߶�ID
	can_send(SPEED_MSG,pack.speed, 8)	;			//SPEED_MSG  �ٶ�ID
	can_send(SP_MSG,pack.sp_seat_env_id,8);     //SP_MSG				  //��ЧID
}
#endif



uint8_t shared_memory_ram[1024] = {0};
ram_t *ram = (ram_t *)shared_memory_ram;
/*50����һ�εĶ������ݷ���*/
void can_action_date_sent(void)
{
	if(get_timer2_enable_can_sent_flag())
	{
		clr_timer2_enable_can_sent_flag();
		buscan_control(ram->high,ram->sp_seat,ram->sp_env,ram->speed,0);
	}				
}	
