#include "kalman.h"

/**
* @brief Computes the kalman estimation (in C)of output based on previous inputs
* @param 	InputArray 		Reference to the array of measurements
* @param 	OutputArray 	Reference to the empty where results will be stored
* @param 	Length 				Length of both arrays 
* @retval 	0	if no error occured
* 					1 if error occured during calculation
*/
int Kalmanfilter_C (float* InputArray, float* OutputArray, kalman_state* kstate, int Length) {
	int i;
	
	
	for (i = 0;i< Length;i++){
		//Equation as given in class
		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x  + kstate->k  *  (InputArray[i] - kstate->x);
		kstate->p = (1.0f - kstate->k) * kstate->p;
		OutputArray[i]=kstate->x;
		
		//If any NaN is found, abort calculation and return that error has occured
		if (OutputArray[i] != OutputArray[i]) { // NaN check
			return 1;
		}
		
	}
	return 0;
}
