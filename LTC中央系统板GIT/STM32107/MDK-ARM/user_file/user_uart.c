#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "user_uart.h"
#include <string.h>

static uint8_t uart2_receive_data = 0U;
void user_uart_init(void)
{
	memset((void *)&frame,0,sizeof(frame));
	__HAL_UART_ENABLE(&huart2);
	__HAL_UART_ENABLE(&huart3);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&(uart2_receive_data), 1);
}

int fputc(int ch, FILE *f)
{
	huart3.Instance->DR = ch;
	while(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TXE) == RESET);
	return ch; 
}

/*显示用途，调试用*/
static uint8_t mark_uart3tx;
void printf_debug_info(void)
{ 	
	if(mark_uart3tx==0|| __HAL_UART_GET_FLAG(&huart3, UART_FLAG_TXE)!=RESET)
	{
		printf("\r\nThe live %d ID is %x\r",mark_uart3tx,stdid_buff[mark_uart3tx]);//打印当前存活的ID号；
    mark_uart3tx++;		
	}
	if(mark_uart3tx==SEAT_AMOUNT)
	{
		mark_uart3tx=0;
		printf("\n");
	}
}

struct frame frame={0};
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART2)
	{
		if(frame.index == 0 && frame.data == 0xff)	//判定帧头0xff
		{
			frame.index++;
			HAL_UART_Receive_IT(&huart2, (uint8_t *)&(frame.data), 1);
			return;
		}
		if(frame.index == 1)	//判定帧头0xc2
		{
			switch (frame.data)
			{
			case 0x4a:
				frame.index++;
				break;
			case 0xff:
				break;
			default :
				frame.index = 0;
			break;
			}
			HAL_UART_Receive_IT(&huart2, (uint8_t *)&(frame.data), 1);
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
			HAL_UART_Receive_IT(&huart2, (uint8_t *)&(frame.data), 1);
			return;
		}
		frame.buff[frame.index] = frame.data;
		frame.index++;

		HAL_UART_Receive_IT(&huart2, (uint8_t *)&(frame.data), 1);
	}
}	

