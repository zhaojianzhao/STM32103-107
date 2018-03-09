#ifndef __USER_IO_H
#define __USER_IO_H
#include "stm32f1xx_hal.h"


#define GET_ID_1() (!HAL_GPIO_ReadPin(INPUT_BCD1_1_GPIO_Port, INPUT_BCD1_1_Pin))
#define GET_ID_2() (!HAL_GPIO_ReadPin(INPUT_BCD2_1_GPIO_Port, INPUT_BCD2_1_Pin))
#define GET_ID_4() (!HAL_GPIO_ReadPin(INPUT_BCD4_1_GPIO_Port, INPUT_BCD4_1_Pin))
#define GET_ID_8() (!HAL_GPIO_ReadPin(INPUT_BCD8_1_GPIO_Port, INPUT_BCD8_1_Pin))
#define GET_ID_10() (!HAL_GPIO_ReadPin(INPUT_BCD1_2_GPIO_Port, INPUT_BCD1_2_Pin))
#define GET_ID_20() (!HAL_GPIO_ReadPin(INPUT_BCD2_2_GPIO_Port, INPUT_BCD2_2_Pin))
#define GET_ID_40() (!HAL_GPIO_ReadPin(INPUT_BCD4_2_GPIO_Port, INPUT_BCD4_2_Pin))
#define GET_ID_80() (!HAL_GPIO_ReadPin(INPUT_BCD8_2_GPIO_Port, INPUT_BCD8_2_Pin))
extern void user_io_init(void);  

#endif
