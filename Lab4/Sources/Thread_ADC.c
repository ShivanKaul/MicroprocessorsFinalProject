/*******************************************************************************
  * @file    Thread_LED.c
  * @author  Ashraf Suyyagh
	* @version V1.0.0
  * @date    17-January-2016
  * @brief   This file initializes one LED as an output, implements the LED thread 
  *					 which toggles and LED, and function which creates and starts the thread	
  ******************************************************************************
  */
	
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"
#include "kalman.h"
#include "stdio.h"
void Thread_ADC (void const *argument);                 // thread function
osThreadId tid_Thread_ADC;                              // thread id
osThreadDef(Thread_ADC, osPriorityAboveNormal, 1, 0);
void poll(void);
osMutexId  alarm_mutex; 
osMutexDef (alarm_mutex); 

/**
* @brief Create temperature the thread within RTOS context
* @param None
* @retval 0 if no errors 
* 				-1 if errors
*/
int start_Thread_ADC (void) {
	alarm_mutex = osMutexCreate(osMutex(alarm_mutex));
  tid_Thread_ADC = osThreadCreate(osThread(Thread_ADC ), NULL); // Start LED_Thread
  if (!tid_Thread_ADC) return(-1); 
  return(0);
}


#define ADC_FLAG 1
/**
* @brief Main loop for the adc
* @param None
* @retval None 
*/
void Thread_ADC (void const *argument) {
		while(1){
			osSignalWait (ADC_FLAG,osWaitForever); 
			osSignalClear(tid_Thread_ADC,ADC_FLAG); 
			poll();
		}
}
	

extern ADC_HandleTypeDef	ADC1_Handle;
extern kalman_state kalman_temp;
float getSetValue(float newValue,int setmode, int index);
	/**
* @brief Polls the ADC for temperature digitization, and filters using Kalman filter
* 		> Set alarm if temperature above threshold
* @file main.c
* @param None
* @retval None
*/
void poll() { 
	
	float  voltage, temperature, filtered_temp;
	HAL_ADC_Start(&ADC1_Handle);
	if (HAL_ADC_PollForConversion(&ADC1_Handle, 10) == HAL_OK)
	{
			voltage = HAL_ADC_GetValue(&ADC1_Handle);
			voltage *= (3000.0f/0xfff); // getting resolution in mV
			temperature = (voltage -760.0f) / 2.5f; // normalizing around 25C voltage and  average slope 
			temperature += 25.0f;
			
			// Use Kalman filter to get filtered value and store in 'filtered_temp'
			Kalmanfilter_C(&temperature, &filtered_temp, &kalman_temp, 1);
		
	//	printf("%f\n", filtered_temp);
			getSetValue(filtered_temp,1,2);
			__HAL_ADC_CLEAR_FLAG(&ADC1_Handle,ADC_FLAG_EOC);
			
	}
}

