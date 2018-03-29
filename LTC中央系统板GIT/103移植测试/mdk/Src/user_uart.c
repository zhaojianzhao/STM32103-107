#include "stm32f1xx_hal.h"
#include "user_uart.h"

#include <string.h>

extern UART_HandleTypeDef huart1;

struct frame frame = {0};

void user_uart_init(void)
{
	memset((void *)&frame, 0, sizeof(frame));
	__HAL_UART_ENABLE(&huart1);
//	HAL_UART_Receive_IT(&huart1, (uint8_t *)&(frame.data), 1);	//串口接收一个字节，并通过中断返回结果
}

void user_uart_stop(void)
{
	__HAL_UART_DISABLE(&huart1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(frame.index == 0 && frame.data == 0xff)	//判定帧头0xff
	{
		frame.index++;
		HAL_UART_Receive_IT(&huart1, (uint8_t *)&(frame.data), 1);
		return;
	}
	if(frame.index == 1)	//判定帧头0xc2
	{
		switch (frame.data)
		{
			case 0xc2:
				frame.index++;
				break;
			case 0xff:
				break;
			default :
				frame.index = 0;
				break;
		}
		HAL_UART_Receive_IT(&huart1, (uint8_t *)&(frame.data), 1);
		return;
	}
	if(frame.index >= 8)	//判定帧尾0xee
	{
		switch (frame.data)
		{
			case 0xee:
				frame.enable = 1;
				frame.index = 0;
				break;
			default :
				frame.index = 0;
				break;
		}
		HAL_UART_Receive_IT(&huart1, (uint8_t *)&(frame.data), 1);
		return;
	}
	frame.buff[frame.index] = frame.data;
	frame.index++;

	HAL_UART_Receive_IT(&huart1, (uint8_t *)&(frame.data), 1);
}


int fputc(int ch, FILE *f)
{
	huart1.Instance->DR = ch;
	while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) == RESET);
	return ch; 
}
