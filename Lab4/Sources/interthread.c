#include "cmsis_os.h" 
#include "interthread.h"
extern osMutexId  disp_mutex; 

extern osMutexId  alarm_mutex; 

extern osMutexId  button_mutex;


float displayed_values[3]; 
/*
   * @brief Sets the values at the specific index and then sets it
   * disp_mutex locks access to this function and variables
   * @param newValue	The value at which to set 
	 * @param setmode		0 to get 
											1 to get
	* @param setmode		0 for roll 
											1 for pitch
											2 for temperature
   * @retval The current value that was retrieved or set
*/
float getSetValue(float newValue,int setmode, int index){
	osMutexWait(disp_mutex,osWaitForever); 
	if (setmode){
		displayed_values[index]=newValue;
	}
	newValue = displayed_values[index];
	osMutexRelease(disp_mutex); 
	return newValue;
}

int alarm_flag;
int getSetAlarm(int newValue, int setmode){
	osMutexWait(alarm_mutex,osWaitForever); 
	if (setmode){
		alarm_flag=newValue;
	}
	newValue = alarm_flag;
	osMutexRelease(alarm_mutex); 
	return newValue;
}


int buttonLastPressed;
/*
   * @brief Sets the current channel used according to  the button press
   * @param button	The value at which to set 
	 * @param setmode		0 to get 
											1 to get
   * @retval The current value that was retrieved or set
*/
int getSetButton(int button, int setmode){
	osMutexWait(button_mutex,osWaitForever); 
	// set by keypad
	if (setmode){
		buttonLastPressed = button;
	}
	button = buttonLastPressed;
	osMutexRelease(button_mutex); 
	return button;
}

