#ifndef __SW_TIMER_H
#define __SW_TIMER_H
#include "stm32f1xx_hal.h"

extern void task_can_tx(void);
extern void task_100ms_prf(void);
#define 	TASK_TBALE  \
					{0,&task_can_tx,10,1000},\
					{0,&task_100ms_prf,100,1000}
		
extern void sw_timer_init(void);		
extern void sw_timer_handle(void);	
extern void can_tx_server_init(void);	
extern void can_rx_server_init(void);						
#endif
