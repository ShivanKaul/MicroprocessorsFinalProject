/**
 * @brief Keypad Handling logic
 * @author Yusaira Khan 
 * @author Shivan Kaul
 */
 
#include "keypad.h"
#include "stdio.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "interthread.h"
osThreadId tid_Thread_Keypad;
void Thread_Keypad(void const *argument);
osThreadDef(Thread_Keypad, osPriorityBelowNormal, 1, 0);

osMutexId  button_mutex; 
osMutexDef (button_mutex); 
#define NOREAD 100

/**
   * @brief Create keypad thread
   * @param None
   * @retval The correct value of button read or Error code NOREAD
   */
int start_Thread_Keypad	(void){
	tid_Thread_Keypad = osThreadCreate(osThread(Thread_Keypad), NULL); // Start thread
  if (!tid_Thread_Keypad) return(-1); 
  return(0);
}

int buttonPressed = 0, keypad_flag = 0;
/**
   * @brief Keypad thread
   * @param arguments
   * @retval None
   */
void Thread_Keypad(void const *argument){
	while (1) {
		osDelay(1);
		keypad_flag = !keypad_flag;
		buttonPressed = readButton();
		if (buttonPressed != NOREAD) { // Button is pressed
			getSetButton(buttonPressed, 1);
		}
	}
}

/* Definitions */
#define keypadGPIO GPIOE

// Buttons
#define button1 0xee00
#define button4 0xde00
#define button7 0xbe00
#define buttonE 0x7e00
#define button2 0xed00
#define button5 0xdd00
#define button8 0xbd00
#define buttonB 0xd700
#define button3 0xeb00
#define button6 0xdb00
#define button9 0xbb00
#define buttonF 0x7b00
#define buttonA 0xe700 // Enter button
#define button0 0x7d00
#define buttonC 0xb700
#define buttonD 0x7700
#define INITREAD 0
#define DEBOUNCE_DELAY 300

/* Variables */
int no_debouncing_flag = 0;
int debouncing_countdown = 0;
extern int keypad_flag;
uint16_t read = INITREAD;
GPIO_InitTypeDef GPIO_Init_Keypad_Row,GPIO_Init_Keypad_Col ;

/**
   * @brief Initialize the GPIOs for the Keypad
   * @param None
   * @retval None
   */
void init_keypad(void){
	__HAL_RCC_GPIOE_CLK_ENABLE();
	GPIO_Init_Keypad_Row.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 ;
	setInput(&GPIO_Init_Keypad_Row);
	GPIO_Init_Keypad_Col.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	setOutput(&GPIO_Init_Keypad_Col);
}


/**
   * @brief Read the button pressed on the keypad by polling
   * @param None
   * @retval The correct value of button read or Error code NOREAD
   */
int keypad_flag;
uint8_t readButton(void){
	
	if (keypad_flag) {
		read |= ((keypadGPIO->IDR) & GPIO_Init_Keypad_Row.Pin); //read only row pins and get all 0s
		if (read!=(INITREAD|GPIO_Init_Keypad_Row.Pin)){//button press
			if (!debouncing_countdown) {// not zero
				//printf("rows %d\n",read);
				setInput(&GPIO_Init_Keypad_Col);
				setOutput	(&GPIO_Init_Keypad_Row);
			} else {
				keypad_flag=!keypad_flag;
				read=INITREAD;
			}
			
		} else {
			keypad_flag=!keypad_flag;
			read=INITREAD;
			debouncing_countdown= debouncing_countdown<1 ? 0 : debouncing_countdown -1;
		}
		return NOREAD;
	} else { // need to find column now!
		uint16_t temp;
		read |= ((keypadGPIO->IDR)& GPIO_Init_Keypad_Col.Pin); //read only col pins and get all 0s
		setInput(&GPIO_Init_Keypad_Row);
		setOutput(&GPIO_Init_Keypad_Col);
		debouncing_countdown=DEBOUNCE_DELAY;
		temp = read;
		read = INITREAD;
		switch (temp){
			case button1: return 0;
			case button2: return 1;
			case button3: return 2;
			default: return NOREAD;
		}
	}
}

/**
   * @brief Set the relevant GPIO pins to be input
   * @param Keypad_Input	the GPIO init struct that stores the pins used
   * @retval None
   */
void setInput(GPIO_InitTypeDef *Keypad_Input){
	Keypad_Input->Speed = GPIO_SPEED_FREQ_LOW;
	Keypad_Input->Mode = GPIO_MODE_INPUT;
	Keypad_Input->Pull = GPIO_PULLUP;
	HAL_GPIO_Init(keypadGPIO, Keypad_Input);
}

/**
   * @brief Set the relevant GPIO pins to be output
   * @param Keypad_Output	the GPIO init struct that stores the pins used
   * @retval None
   */
void setOutput(GPIO_InitTypeDef* Keypad_Output){
	Keypad_Output->Speed = GPIO_SPEED_FREQ_LOW;
	Keypad_Output->Mode = GPIO_MODE_OUTPUT_PP;
	Keypad_Output->Pull = GPIO_NOPULL;
	HAL_GPIO_Init(keypadGPIO, Keypad_Output);
}
