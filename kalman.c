// kalman.c

/* A generic implementation of a kalman filter to be used to make 
 * estimations of the true values produced by the IMU  to filter 
 * yaw, pitch, and roll
 */ 


#include "kalman.h"

float kalmanGain(float eEST, float eMEA) {
	return eEST/(eEST+eMEA);
}

float kalmanEstimate(float prevEST, float eEST, float eMEA, float measurement) {
	return prevEST + kalmanGain(eEST, eMEA) * (measurement - prevEST);
}

float newESTerror(float prevESTe, float eMEA) {
	return (prevESTe * eMEA)/(eMEA + prevESTe);
}
