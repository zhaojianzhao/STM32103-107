#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "user_time.h"

void user_time_init(void)
{ 

}
static uint8_t tick_flag;
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
