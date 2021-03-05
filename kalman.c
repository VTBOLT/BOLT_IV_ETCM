// kalman.c

/* A generic implementation of a kalman filter to be used to make 
 * estimations of the true values produced by the IMU  to filter 
 * yaw, pitch, and roll
 */ 


#include "kalman.h"

volatile float kalmanGain(volatile float eEST, volatile float eMEA) {
	return eEST/(eEST+eMEA);
}

volatile float kalmanEstimate(volatile float prevEST, volatile float eEST, volatile float eMEA, volatile float measurement) {
	return prevEST + kalmanGain(eEST, eMEA) * (measurement - prevEST);
}

volatile float newESTerror(volatile float prevESTe, volatile float eMEA) {
	return (prevESTe * eMEA)/(eMEA + prevESTe);
}
