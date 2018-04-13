#include "sw_timer.h"


/***************定义时间事件包含的要素的结构体***************/
typedef struct
{
	uint32_t timer;          //保存时间事件的时间计数；
	void (*p_fun_task)(void);//设置时间任务的函数指针；
	uint32_t period;         //设置时间事件的周期；
	uint32_t offset;         //初始的延时时间设置；
}task_timer_t;	

/***************定义时间事件各要素的初始化值***************/
task_timer_t task_timer_table[]=
{
	TASK_TBALE 
};

/***************先初始化赋值时间时间的计数寄存器***************/
void sw_timer_init(void)
{
	uint8_t task_id;
	for(task_id=0;
			task_id<sizeof(task_timer_table)/sizeof(task_timer_table[0]);
			task_id++)																				//初始化各个时间的参数；
	{
		task_timer_table[task_id].timer=HAL_GetTick();      //获取当前的SYSTICK的值；
	}
	can_tx_server_init();
	can_rx_server_init(); 
	hb_item_init(); 
	can_action_tx_init();
}

/***************时间事件的进程***************/
void sw_timer_handle(void)
{
	uint8_t task_id;
	for(task_id=0;
			task_id<sizeof(task_timer_table)/sizeof(task_timer_table[0]);
			task_id++)																				
	{
		if(task_timer_table[task_id].offset!=0)
		{
			if((HAL_GetTick()-task_timer_table[task_id].timer)>=task_timer_table[task_id].offset)
			{
				task_timer_table[task_id].timer=HAL_GetTick();				
				task_timer_table[task_id].offset=0;
				task_timer_table[task_id].p_fun_task();
			}
		}
	  else
	  {
			if(HAL_GetTick()-task_timer_table[task_id].timer>=task_timer_table[task_id].period)
			{
				task_timer_table[task_id].timer=HAL_GetTick();
				task_timer_table[task_id].p_fun_task();
			}	
	  }	
	}	
}
