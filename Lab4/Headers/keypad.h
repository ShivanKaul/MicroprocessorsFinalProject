#include "stm32f4xx_hal.h"
#define NOREAD 100
void init_keypad(void);
void setInput(GPIO_InitTypeDef* );
void setOutput(GPIO_InitTypeDef* );
uint8_t readButton(void);
