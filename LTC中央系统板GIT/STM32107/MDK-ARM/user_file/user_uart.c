#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "user_uart.h"

void user_uart_init(void)
{
	__HAL_UART_ENABLE(&huart3);
}

int fputc(int ch, FILE *f)
{
	huart3.Instance->DR = ch;
	while(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TXE) == RESET);
	return ch; 
}

void printf_debug_info(void)
{ 
	uint8_t i;
	for(i=0;i<SEAT_AMOUNT;i++) 
	{
		printf("\r\nThe live %d ID is %x\r",i+1,StdId_buff[i]);//打印当前存活的ID号；
	}
	printf("\n");
}
