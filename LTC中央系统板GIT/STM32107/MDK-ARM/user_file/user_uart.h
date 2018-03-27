#ifndef __USER_UART_H
#define __USER_UART_H
#include "stm32f1xx_hal.h"


#define UART_BUFF_SIZE 20

struct frame
{
	__IO uint8_t enable;
	__IO uint8_t data;
	__IO uint8_t buff[UART_BUFF_SIZE];
	__IO uint8_t index;
};

extern struct frame frame;
extern void user_uart_init(void);  
extern void printf_debug_info(void);
#endif
