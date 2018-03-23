#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "user_time.h"

static uint8_t can_sent_flag=0;
static uint8_t timer4_heart_beat_flag=0;
void user_time_init(void)
{ 
	HAL_TIM_Base_Start_IT(&htim2);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{   
	if(htim->Instance==TIM2)
	{
		SAFE(can_sent_flag=1);  //50MS发送一次CAN的数据；
	}	
}	

uint8_t get_heart_beat_flag(void)  
{
	uint8_t timer4_flag;
	SAFE(timer4_flag=timer4_heart_beat_flag);
	return timer4_flag;
}

uint8_t get_can_sent_flag(void)
{
	uint8_t timer2_flag;
	SAFE(timer2_flag=can_sent_flag);
	return timer2_flag;
}

void clr_heart_beat_flag(void)
{
	SAFE(timer4_heart_beat_flag=0);
}

void clr_can_sent_flag(void)
{
	SAFE(can_sent_flag=0);
}

uint8_t tick_flag;
void HAL_SYSTICK_Callback(void)
{
	SAFE(tick_flag=1);
}	

uint8_t get_tick_flag(void)
{
	uint8_t flag;
	flag=tick_flag;
	return flag;
}

void clr_tick_flag(void)
{
	SAFE(tick_flag=0);
}
