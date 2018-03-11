#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "user_time.h"

uint8_t timer4_enable_heart_beat_flag=0;
void user_time_init(void)
{ 
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim4);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{   
	if(htim->Instance==TIM2)
	{

	}	
	/*2秒一次发送轮询的座椅信号*/
	if(htim->Instance==TIM4)
	{
		timer4_enable_heart_beat_flag=1;  //使能发送“心跳”信号轮询；
	}		
}	

