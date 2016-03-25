#include "stm32f4xx_hal.h"
#include "supporting_functions.h"

void updateDisplay(void);
int getDecimalPointPosition(int);
uint32_t getRegisterLEDValue(int num,int place,int);

void alarm_on(void);
void alarm_off(void);

