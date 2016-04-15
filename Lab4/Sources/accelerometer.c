#define ARM_MATH_CM4
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"
#include "kalman.h"
#include "lis3dsh.h"
#include "arm_math.h"
#include "math.h"
#include "interthread.h"
#define DOUBLE_TAP_CTR_THRES_FIRST       1
#define DOUBLE_TAP_CTR_THRES_SCND        3

// Variables + function names
osThreadId tid_Thread_Accelerometer;
void Thread_Accelerometer(void const *argument);
osThreadDef(Thread_Accelerometer, osPriorityNormal, 1, 0);
void convertAccToAngle(float* acc, float* angles);
extern arm_matrix_instance_f32 x_matrix,w_matrix,y_matrix;
int is_outlier(float variance);
void calculateAngles (void);
int calc_double_tap_ctr_threshold(float* out);
int detect_double_tap(float* out);

uint8_t DOUBLE_TAP_SIGNAL = 0;
int DOUBLE_TAP_CTR = 0,DOUBLE_TAP_Flag=0;

float current_angle;

float ENTER_DOUBLE_TAP;
float meann = 1022.0;
float stdd = 42.7030;

#define data_ready_flag 1

/**
  * @brief  Start accelerometer thread
  * @param  None
  * @retval int
  */
int start_Thread_Accelerometer	(void){
	tid_Thread_Accelerometer = osThreadCreate(osThread(Thread_Accelerometer ), NULL); // Start LED_Thread
  if (!tid_Thread_Accelerometer) return(-1); 
  return(0);
}

/**
  * @brief  Accelerometer thread
  * @param  arguments
  * @retval None
  */
void Thread_Accelerometer(void const *argument){
	ENTER_DOUBLE_TAP = 0;
	while(1){
		osSignalWait (data_ready_flag,40); 
		osSignalClear(tid_Thread_Accelerometer,data_ready_flag); 
		calculateAngles();
	}
	
}
extern float setAcceleration(float* newValues, int double_tap);
extern float out[], acc[];
extern kalman_state kalman_x, kalman_y,kalman_z;
/**
   * @brief Calculates angles - filter them and use calibration matrix
   * POSITIONING_AXIS specifies which axis to use for positioning
   * @param None
   * @retval None
   */
void calculateAngles (void) {
	float angles[3];
	float magnitude;
	float variance;
	int ctr = 0;
	
	// NOTE1: We initially read acceleration in the interrupt, but on discussion
	// with the TA on Friday, we've pushed it over here for the purposes of 
	// the code submission.
	LIS3DSH_ReadACC(out);
		
	Kalmanfilter_C (out, out, &kalman_x, 1);
	Kalmanfilter_C (out+1, out+1, &kalman_y, 1);
	Kalmanfilter_C (out+2, out+2, &kalman_z, 1);
	arm_mat_mult_f32(&w_matrix,&x_matrix,&y_matrix);
	convertAccToAngle(acc, angles);
		
	DOUBLE_TAP_SIGNAL = detect_double_tap(out);	
	// Get angles
	//if (DOUBLE_TAP_SIGNAL>0)
	//printf("DOUBLE TAP SIGNAL = %d \n\n", DOUBLE_TAP_SIGNAL);
	
	if(DOUBLE_TAP_SIGNAL){
		while(ctr < 5){
			setAcceleration(angles,1);
			ctr++;
		}
	}else{
		setAcceleration(angles,0);
	}
	
	
	

}

int detect_double_tap(float* out){
	float magnitude = sqrt(pow(*out,2) + pow(*(out+1),2) + pow(*(out+2),2));
	float variance = fabs(magnitude - meann);
	
	if(is_outlier(variance)){
		ENTER_DOUBLE_TAP = 1;
	}
	
	if(ENTER_DOUBLE_TAP){
		DOUBLE_TAP_CTR++;
	}
	
	if(DOUBLE_TAP_CTR >= DOUBLE_TAP_CTR_THRES_FIRST){
		if(is_outlier(variance)){
			printf("DOUBLE TAP %d\n",DOUBLE_TAP_CTR);
			DOUBLE_TAP_Flag=1;
			return 1;
		}
		
		if(DOUBLE_TAP_CTR >= DOUBLE_TAP_CTR_THRES_SCND){
			DOUBLE_TAP_CTR = 0;
			ENTER_DOUBLE_TAP = 0;
			DOUBLE_TAP_Flag=0;
		}
	}
	
	//printf("DOUBLE_TAP_CTR = %d \n\n",DOUBLE_TAP_CTR);
	
	return 0;
}

int calc_double_tap_ctr_threshold(float* out){
	float magnitude = sqrt(pow(*out,2) + pow(*(out+1),2) + pow(*(out+2),2));
	float variance = fabs(magnitude - meann);
	
	if(is_outlier(variance)){
		DOUBLE_TAP_CTR++;
	}
	
//	else{
//		printf("DOUBLE TAP = %d\n\n", DOUBLE_TAP_CTR);
//		DOUBLE_TAP_CTR = 0;
//	}
	
//	if(DOUBLE_TAP_CTR >= DOUBLE_TAP_CTR_THRESHOLD){
//		if(is_outlier(variance)){
//			printf("DOUBLE TAP!");
//			DOUBLE_TAP_CTR = 0;
//			return 1;
//		}
//	}
	
	return 0;
}

/**
* @brief Get an Angle through arctan depending on all 3 accelerations for accuracy
* @file mathhelper.c
* @param num	numerator for the tangent
* @param denom1	one of the axes in the denominator
* @param denom1	the other axes in the denominator
* @retval angle that was calculated
*/
float getArcTan(float num, float denom1, float denom2){
	//getting arc COT of angle
	float angle=atan2(sqrt(denom1*denom1 + denom2*denom2),num )*180/3.14159265;
	//setting 90 as vertical
	angle=angle-90;
	if (denom2 < 0){
		angle= -angle;
	}
	
	//getting rid of negative angles since tan x == tan (180 +x)
	if (angle < 0){
		return angle + 180;
	}
	return angle;
}
/**
* @brief Get an Angle through arctan depending on all 3 accelerations for accuracy
* @file mathhelper.c
* @param acc accelaration array pointer
* @param angles results pointers
* @retval None
*/
void convertAccToAngle(float* acc, float* angles) {
	angles[0] = getArcTan(acc[0],acc[1], acc[2]); //roll calculation
	
	
	//Used in pitch and yaw calculations
	angles[1] = getArcTan(acc[1],acc[0], acc[2]);
	// angles[2] = getArcTan(acc[2],acc[0], acc[1]);
}

// Helper function
float absolute(float x) {
	return x >= 0 ? x : -x;
}

// Helper function
int is_outlier(float variance){
	return (variance >= 2.50f*stdd) && (variance <= 3.00f*stdd);
} 



