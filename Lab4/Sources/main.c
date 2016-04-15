/**
 * @brief Main file
 * @author Yusaira Khan 
 * @author Shivan Kaul
 */

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "RTE_Components.h"             // Component selection
#include "init.h"
#include "kalman.h"
#include "keypad.h"

#include <stdio.h>
#include "stm32f4xx.h"

#define LED_TOGGLE_UNIT									1

int fputc(int c, FILE *stream)
{
   return(ITM_SendChar(c));
}
extern int start_Thread_ADC			(void);
extern void Thread_ADC(void const *argument);
extern osThreadId tid_Thread_ADC;

extern int start_Thread_Accelerometer			(void);
extern void Thread_Accelerometer(void const *argument);
extern osThreadId tid_Thread_Accelerometer;

extern int start_Thread_Bluetooth			(void);
extern void Thread_Bluetooth(void const *argument);
extern osThreadId tid_Thread_Bluetooth;

extern int start_Thread_LED			(void);
extern void Thread_LED(void const *argument);
extern osThreadId tid_Thread_LED;

extern TIM_Base_InitTypeDef Timinit;
extern TIM_OC_InitTypeDef TIM_LED_Channel_config;
extern TIM_HandleTypeDef TIM_LED_handle;

void set_pwm(int);
void toggle_clockwise(void);
void toggle_anticlockwise(void);
void toggle_LEDS_off(void);
void toggle_LEDS_on(void);

int CTRL_LEDS = 0;
int CTRL_PWM = 0;
int CTRL_PWM_LAST = 0;

/**
	These lines are mandatory to make CMSIS-RTOS RTX work with te new Cube HAL
*/
#ifdef RTE_CMSIS_RTOS_RTX
extern uint32_t os_time;

uint32_t HAL_GetTick(void) { 
  return os_time; 
}
#endif

kalman_state kalman_temp = INIT_KALMAN;

/**
  * Main function
  */
	int cntr = 0;
int main (void) {
	osThreadId main_id;
  	osKernelInitialize();                     /* initialize CMSIS-RTOS          */

  	HAL_Init();                               /* Initialize the HAL Library     */

  	SystemClock_Config();                     /* Configure the System Clock     */
	
	// Initialize 
	gpioInit();
	TIM_LED_PWM_Init();
	TIM_ADC_Init();
	LISInit();
	ADCInit();
	kalman_init();
	//init_keypad();
	matrix_init();
	
	main_id = osThreadGetId(); // Get thread id for main
	
	// Initialize all threads
	start_Thread_Bluetooth();
	start_Thread_ADC(); 
	//start_Thread_7Seg();
	start_Thread_Accelerometer();
	//start_Thread_LED();	
	
	osKernelStart();                          /* start thread execution         */
	//osThreadTerminate(main_id);

	while(1){
		//printf("CTRL_LEDS = %d, CTRL_PWM = %d\n\n", CTRL_LEDS, CTRL_PWM);
		
		if(CTRL_PWM_LAST != CTRL_PWM){
			set_pwm(CTRL_PWM);
			CTRL_PWM_LAST = CTRL_PWM;
		}
		
		switch(CTRL_LEDS){
			case 0:
				toggle_LEDS_off();
				break;
			case 1:
				toggle_clockwise();
				break;
			case 2:
				toggle_anticlockwise();
				break;
			case 3:
				toggle_LEDS_on();
				break;
		}
		
		osDelay(700);
	}
}

void set_pwm(int pwm){
	TIM_LED_Channel_config.Pulse = pwm*100;
	
	//HAL_TIM_PWM_Init(&TIM_LED_handle);
	HAL_TIM_PWM_ConfigChannel(&TIM_LED_handle, &TIM_LED_Channel_config, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&TIM_LED_handle, &TIM_LED_Channel_config, TIM_CHANNEL_2);
	HAL_TIM_PWM_ConfigChannel(&TIM_LED_handle, &TIM_LED_Channel_config, TIM_CHANNEL_3);
	HAL_TIM_PWM_ConfigChannel(&TIM_LED_handle, &TIM_LED_Channel_config, TIM_CHANNEL_4);
}

void toggle_clockwise(void){
	HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_4);
	
	if(cntr <= LED_TOGGLE_UNIT){
		HAL_TIM_PWM_Start(&TIM_LED_handle, TIM_CHANNEL_1);
	}else if(cntr <= LED_TOGGLE_UNIT*2 && cntr > LED_TOGGLE_UNIT){
		//HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&TIM_LED_handle, TIM_CHANNEL_2);
	}else if(cntr > LED_TOGGLE_UNIT*2 && cntr <= LED_TOGGLE_UNIT*3){
		//HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&TIM_LED_handle, TIM_CHANNEL_3);
	}else{
		//HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_3);
		HAL_TIM_PWM_Start(&TIM_LED_handle, TIM_CHANNEL_4);
	}
	
	//osDelay(50);

	if(cntr > LED_TOGGLE_UNIT*4){
		cntr = 0;
		//HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_4);
	}
	
	cntr++;
	
	//printf("CNTR = %d\n\n",cntr);
}

void toggle_anticlockwise(void){
	HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_4);
	
	if(cntr <= LED_TOGGLE_UNIT){
		HAL_TIM_PWM_Start(&TIM_LED_handle, TIM_CHANNEL_4);
	}else if(cntr <= LED_TOGGLE_UNIT*2 && cntr > LED_TOGGLE_UNIT){
		//HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_4);
		HAL_TIM_PWM_Start(&TIM_LED_handle, TIM_CHANNEL_3);
	}else if(cntr > LED_TOGGLE_UNIT*2 && cntr <= LED_TOGGLE_UNIT*3){
		//HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_3);
		HAL_TIM_PWM_Start(&TIM_LED_handle, TIM_CHANNEL_2);
	}else{
		//HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&TIM_LED_handle, TIM_CHANNEL_1);
	}
	
	//osDelay(50);
	
	if(cntr > LED_TOGGLE_UNIT*4){
		cntr = 0;
		//HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_1);
	}
	
	cntr++;
	
	//printf("CNTR = %d\n\n",cntr);
}

void toggle_LEDS_on(void){
	HAL_TIM_PWM_Start(&TIM_LED_handle, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TIM_LED_handle, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&TIM_LED_handle, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&TIM_LED_handle, TIM_CHANNEL_4);
}

void toggle_LEDS_off(void){
	HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&TIM_LED_handle, TIM_CHANNEL_4);
}
