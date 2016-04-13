#include "cmsis_os.h" 
#include "interthread.h"
extern osMutexId  disp_mutex; 

extern osMutexId  alarm_mutex; 

extern osMutexId  button_mutex;


float displayed_values[3]; 
int double_tap_signal;

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


float setTemperature(float newValue){
	osMutexWait(disp_mutex,osWaitForever); 
	displayed_values[2]=newValue;
	osMutexRelease(disp_mutex); 
	return newValue;
}
float getSetValue(float newValue,int setmode, int index){
	//osMutexWait(disp_mutex,osWaitForever); 
	if (setmode){
		displayed_values[index]=newValue;
	}
	newValue = displayed_values[index];
	//osMutexRelease(disp_mutex); 
	return newValue;
}

float setAcceleration(float* newValues, int double_tap){
	osMutexWait(disp_mutex,osWaitForever); 
	displayed_values[0]=newValues[0];
	displayed_values[1]=newValues[1];
	double_tap_signal = double_tap;
	osMutexRelease(disp_mutex); 
	return 0;
}
uint32_t bluetooth_data[4];
uint8_t* getBluetooth(){
	osMutexWait(disp_mutex,osWaitForever); 
	bluetooth_data[0]=(int)(displayed_values[0]*100);
	bluetooth_data[1]=(int)(displayed_values[1]*100);
	bluetooth_data[2]=(int)(displayed_values[2]*100);
	bluetooth_data[3]=double_tap_signal ;
	osMutexRelease(disp_mutex); 
	return (uint8_t*)bluetooth_data;
}


