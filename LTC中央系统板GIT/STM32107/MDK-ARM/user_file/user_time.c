#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "user_time.h"

static uint8_t timer2_enable_can_sent_flag=0;
static uint8_t timer4_enable_heart_beat_flag=0;
void user_time_init(void)
{ 
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim4);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{   
	if(htim->Instance==TIM2)
	{
		SAFE(timer2_enable_can_sent_flag=1);  //50MS����һ��CAN�����ݣ�
	}	
	/*2��һ�η�����ѯ�������ź�*/
	if(htim->Instance==TIM4)
	{
		SAFE(timer4_enable_heart_beat_flag=1);  //ʹ�ܷ��͡��������ź���ѯ��
	}		
}	
uint8_t get_timer4_enable_heart_beat_flag(void)
{
	uint8_t timer4_flag;
	SAFE(timer4_flag=timer4_enable_heart_beat_flag);
	return timer4_flag;
}

uint8_t get_timer2_enable_can_sent_flag(void)
{
	uint8_t timer2_flag;
	SAFE(timer2_flag=timer2_enable_can_sent_flag);
	return timer2_flag;
}

void clr_timer4_enable_heart_beat_flag(void)
{
	SAFE(timer4_enable_heart_beat_flag=0);
}

void clr_timer2_enable_can_sent_flag(void)
{
	SAFE(timer2_enable_can_sent_flag=0);
}

