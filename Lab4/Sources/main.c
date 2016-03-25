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


extern int start_Thread_ADC			(void);
extern void Thread_ADC(void const *argument);
extern osThreadId tid_Thread_ADC;

extern int start_Thread_Accelerometer			(void);
extern void Thread_Accelerometer(void const *argument);
extern osThreadId tid_Thread_Accelerometer;


// Variables

extern int start_Thread_7Seg			(void);
extern void Thread_7Seg(void const *argument);
extern osThreadId tid_Thread_7Seg;

extern int start_Thread_Keypad	(void);
extern void Thread_Keypad(void const *argument);
extern osThreadId tid_Thread_Keypad;
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
int main (void) {
	osThreadId main_id;
  	osKernelInitialize();                     /* initialize CMSIS-RTOS          */

  	HAL_Init();                               /* Initialize the HAL Library     */

  	SystemClock_Config();                     /* Configure the System Clock     */
	
	// Initialize 
	TIM_LED_Init();
	TIM_ADC_Init();
	LISInit();
	ADCInit();
	kalman_init();
	init_keypad();
	matrix_init();
	gpioInit();
	
	main_id = osThreadGetId(); // Get thread id for main
	
	// Initialize all threads
	start_Thread_ADC(); 
	start_Thread_7Seg();
	start_Thread_Accelerometer();
	start_Thread_Keypad();	
	
	osKernelStart();                          /* start thread execution         */
	osThreadTerminate(main_id);
}
