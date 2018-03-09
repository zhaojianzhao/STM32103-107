#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "user_time.h"

void user_time_init(void)
{ 
	HAL_TIM_Base_Start_IT(&htim2);

}
/*每秒打印存活的座椅ID号*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{   
		uint8_t i;
	  if(htim->Instance==TIM2)
		{
	      for(i=0;i<seat_amount;i++)
				{
					printf("\r\nThe live %d ID is %x\r",i+1,StdId_buff[i]);//打印当前存活的ID号；
				}
		}	
		
}	
