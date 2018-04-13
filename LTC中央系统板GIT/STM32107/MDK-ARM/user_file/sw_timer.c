#include "sw_timer.h"


/***************����ʱ���¼�������Ҫ�صĽṹ��***************/
typedef struct
{
	uint32_t timer;          //����ʱ���¼���ʱ�������
	void (*p_fun_task)(void);//����ʱ������ĺ���ָ�룻
	uint32_t period;         //����ʱ���¼������ڣ�
	uint32_t offset;         //��ʼ����ʱʱ�����ã�
}task_timer_t;	

/***************����ʱ���¼���Ҫ�صĳ�ʼ��ֵ***************/
task_timer_t task_timer_table[]=
{
	TASK_TBALE 
};

/***************�ȳ�ʼ����ֵʱ��ʱ��ļ����Ĵ���***************/
void sw_timer_init(void)
{
	uint8_t task_id;
	for(task_id=0;
			task_id<sizeof(task_timer_table)/sizeof(task_timer_table[0]);
			task_id++)																				//��ʼ������ʱ��Ĳ�����
	{
		task_timer_table[task_id].timer=HAL_GetTick();      //��ȡ��ǰ��SYSTICK��ֵ��
	}
	can_tx_server_init();
	can_rx_server_init(); 
	hb_item_init(); 
	can_action_tx_init();
}

/***************ʱ���¼��Ľ���***************/
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
