#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "user_time.h"

void user_time_init(void)
{ 
	HAL_TIM_Base_Start_IT(&htim2);

}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{   
	  if(htim->Instance==TIM2)
		{

		}	
		
}	
