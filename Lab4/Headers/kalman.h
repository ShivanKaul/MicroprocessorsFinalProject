typedef struct {
	float q; /**< process noise variance */
	float r; /**< measurement noise variance */
	float x; /**< value */
	float p; /**< estimation error variance */
	float k; /**< kalman gain */
} kalman_state;
int Kalmanfilter_C (float* InputArray, float* OutputArray, kalman_state* kstate, int Length);
#define INIT_KALMAN {0.5, 100, 0, 1000, 0}
