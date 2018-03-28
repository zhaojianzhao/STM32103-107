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

/*32λ�������ģʽ*/
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
     
  sfilterconfig.FilterNumber = 0;               //ʹ�ù�����0  
  sfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;     //����Ϊ����ģʽ  
  sfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;    //����Ϊ32λ��  
  sfilterconfig.FilterIdHigh =(stdid_array[0]<<5);     //��֤���������ΪStdIdArray[]����������һ��������ʹ��StdIdArray[0]��Ϊ��֤��  
  sfilterconfig.FilterIdLow =0;  
    
  mask =0x7ff;                      //���濪ʼ����������  
  num =sizeof(stdid_array)/sizeof(stdid_array[0]);  
  for(i =0; i<num; i++)      //������λStdIdArray[]���������г�Ա��ͬ����  
  {  
    tmp =stdid_array[i] ^ (~stdid_array[0]);  //���������Ա���0����Ա����ͬ�����  
    mask &=tmp;  
  }  
  sfilterconfig.FilterMaskIdHigh =(mask<<5);  
  sfilterconfig.FilterMaskIdLow =0|0x02;        //ֻ��������֡  
    
  sfilterconfig.FilterFIFOAssignment = 0;       //����ͨ��������֡���뵽FIFO0��  
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
/*2��һ������������ѯģ�飬���Ҷ���FLAG����־ĳ������ȱ���Ĵ�������¼*/
void heart_beat_checkout(void)
{
	uint8_t i;
	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
	for(i=0;i<SEAT_AMOUNT;i++)
	{
		if(stdid_buff[i]==0)        //�ж�ĳ�������Ƿ�ȱϯ��
		{
			status.hb_count[i]++;			//ȱϯ������һ��
			if(status.hb_count[i]>=3)
			{
				stdid_buff[i]=0;         //��ȱϯ�Ĵ������ڵ���3�ξ�Ĭ��ȱϯ��
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
	hcan1.pTxMsg->StdId=msg_id; /*����Ҫ�������ݵ�Ŀ���ַ*/
	hcan1.pTxMsg->DLC=len;
	for(i=0;i<len;i++)
	{
		hcan1.pTxMsg->Data[i]=data[i];
	}
	if(HAL_CAN_Transmit(&hcan1, 1)!=HAL_OK)
	{
		; /* do nothing */
	} 
	CAN1->IER|=(1<<1);   //��ֹ�Ͽ���IER^2λ��λ��
}

/*����SEAT_AMOUNT �ε���ѯ���������źŸ����Σ�ͨ��can_send_buff[0]�������ε�ַ  can_send_buff[1]���������ź�  can_send_buff[2]������֤��*/
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
		/*�����⵽��STDID����#define HEART_BEAT 0x200  ������ID�ţ�*/
		if(((hcan1.pRxMsg->StdId&HEART_BEAT)==HEART_BEAT)&&(hcan1.pRxMsg->Data[1]==0x01)&&(hcan1.pRxMsg->Data[2]==0x55))
		{
		 stdid_buff[(hcan1.pRxMsg->StdId-HEART_BEAT)]=	hcan1.pRxMsg->StdId;
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
						can_send(HIGHT_MSG_ID,pack.high,3);    //�ȷ�	HIGHT_MSG_ID=0x100,  //�߶�ID
						break;
		case 1: mark_cantx++;
						can_send(SPEED_MSG_ID,pack.speed, 3)	;			//SPEED_MSG_ID  �ٶ�ID
						break;
		case 2: mark_cantx=0;
						clr_can_sent_flag();
						frame.enable=0;
						can_send(SP_MSG_ID,pack.sp_seat_env_id,3);     //SP_MSG_ID				  //��ЧID
						break;
	}			
}
#endif

uint8_t shared_memory_ram[1024] = {0};
ram_t *ram = (ram_t *)shared_memory_ram;
/*ʱ��Ƕ���¼�*/
static uint32_t time10s;
static uint8_t update;										//�������ݸ��±�־
void time_event(void)
{
	SAFE(update=frame.enable);
	if(get_tick_flag())
	{		
		clr_tick_flag(); 
		time2s++;
		time10s++;
		if(time2s>=2000)      //����2S�����������źţ�
		{
			time2s=0;
			heart_beat_checkout();
		}
		if(time10s>=100)      //����100ms�����ͣ�
		{
			time10s=0;	
			printf_debug_info();
		}	
		
	}	
//	if(get_can_sent_flag())  /*50����һ�εĶ������ݷ���*/
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
	CAN1->IER|=(1<<1); //ȷ��CAN���������Ȳ�Σ�	
}	
